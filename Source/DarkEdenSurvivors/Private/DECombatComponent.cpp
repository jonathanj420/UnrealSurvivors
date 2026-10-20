// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatComponent.h"
#include "DEStatComponent.h"
#include "DEAccessoryComponent.h"
#include "DEHealthComponent.h"
#include "DECharacterBase.h"
#include "DEDamageInstigatorInterface.h"


// Sets default values for this component's properties
UDECombatComponent::UDECombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // 스탯 컴포넌트는 틱 필요 없음 (최적화)

}

void UDECombatComponent::BeginPlay()
{
	Super::BeginPlay();
    CachedStatComp = GetOwner()->FindComponentByClass<UDEStatComponent>();
    CachedAccessoryComp = GetOwner()->FindComponentByClass<UDEAccessoryComponent>();
}

FCombatSnapshot UDECombatComponent::GetCombatSnapshot() const
{
    FCombatSnapshot Snapshot;

    // 1. StatComponent가 없으면 깡통 리턴 (방어 코드)
    if (!CachedStatComp)
    {
        return Snapshot;
    }

    // 2. StatComponent에서 기본 스탯 읽어오기 (패시브, 아이템 적용됨)
    Snapshot.FinalDamageMultiplier = CachedStatComp->DamageMultiplier.GetValue();
    Snapshot.CritChance = CachedStatComp->CritChance.GetValue();
    Snapshot.CritDamageMultiplier = CachedStatComp->CritDamageMultiplier.GetValue();
    Snapshot.CooldownReduction = CachedStatComp->CooldownReduction.GetValue();
    Snapshot.EffectSizeMultiplier = CachedStatComp->AreaSize.GetValue();
    Snapshot.DurationMultiplier = CachedStatComp->Duration.GetValue();
    Snapshot.ProjectileSpeedMultiplier = CachedStatComp->ProjectileSpeed.GetValue();
    Snapshot.BonusAmount = FMath::FloorToInt(CachedStatComp->BonusAmount.GetValue());

    // 피흡이나 넉백 같은 것도 StatComponent에 있다면 여기서 가져옴
    // Snapshot.LifeStealChance = CachedStatComp->LifeStealChance.GetValue(); 

    // 3. AccessoryComponent에서 '조건부 스택' 합산하기 (예: 처치 시 공격력 증가)
    if (CachedAccessoryComp)
    {
    }

    return Snapshot;
}

void UDECombatComponent::HandleDamageDealt(const FDEDamageResult& Result)
{
   // UE_LOG(LogTemp, Warning, TEXT("Handle Damage Dealt By : %s"), *Result.SourceObject->GetName());
    if (Result.FinalDamage <= 0.0f || !Result.Victim) return;

    ADECharacterBase* OwnerCharacter = Cast<ADECharacterBase>(GetOwner());
    if (!OwnerCharacter) return;

    TotalDamageDealt += Result.FinalDamage;

    // --------------------------------------------------------
    // [택배 상자(EventData) 포장]
    // --------------------------------------------------------
    FCombatEventData EventData;
    EventData.Instigator = OwnerCharacter;
    EventData.Target = Result.Victim;
    EventData.SourceObject = Result.SourceObject;
    EventData.DamageAmount = Result.FinalDamage;

    // --------------------------------------------------------
    // [로직 1] 적중 시 (OnHit) 이벤트 방송
    // (예전 ProcessLifeSteal 로직은 이제 여기서 자동으로 튀어나옴)
    // --------------------------------------------------------
    if (Result.bCanTriggerOnHit)
    {
        BroadcastCombatEvent(ECombatEventTrigger::OnHit, EventData);
        //UE_LOG(LogTemp, Log, TEXT("Applied OnHit Effect in CombatComp"));
    }
   

    // --------------------------------------------------------
    // [로직 2] 처치 시 (OnKill) 이벤트 방송
    // (예전 ProcessOnKillEffect 로직도 알아서 튀어나옴)
    // --------------------------------------------------------
    if (Result.bIsDead)
    {
        // 킬 집계와 글로벌 OnKill은 출처와 무관하다. 출처는 스킬별 처치 보상(OnTargetKilled)에만 필요하다
        ++TotalKillCount;
        BroadcastCombatEvent(ECombatEventTrigger::OnKill, EventData);

        // Implements<>는 BP에서 붙인 인터페이스도 true지만 그때 네이티브 Cast는 null이다. Cast 결과로만 판단한다
        if (IDEDamageInstigatorInterface* Instigator = Cast<IDEDamageInstigatorInterface>(Result.SourceObject))
        {
            Instigator->OnTargetKilled(Result);
        }
    }

}



void UDECombatComponent::AddCombatEffect(UDECombatEffect* TemplateEffect)
{
    if (!TemplateEffect) return;

    // 1. 내 주머니에 이미 같은 클래스가 있는지 확인
    for (UDECombatEffect* ExistingEffect : ActiveCombatEffects)
    {
        // 원본과 내 주머니에 있는 게 같은 클래스(종류)라면?
        if (ExistingEffect->GetClass() == TemplateEffect->GetClass())
        {
            if (ExistingEffect->bIsStackable && ExistingEffect->CurrentStack < ExistingEffect->MaxStack)
            {
                ExistingEffect->CurrentStack++;
                ExistingEffect->OnStackUpgraded();
                UE_LOG(LogTemp, Warning, TEXT("Effect %s Level Up: %d"), *ExistingEffect->GetName(), ExistingEffect->CurrentStack);
            }
            return;
        }
    }

    // 2. [핵심] 주머니에 없다면? 데이터 에셋의 '원본(Template)'을 똑같이 복제해서 장착!
    // NewObject 대신 DuplicateObject를 쓰면, 에디터에서 입력해 둔 Chance나 Value 값이 그대로 복사됩니다!
    UDECombatEffect* NewEffect = DuplicateObject<UDECombatEffect>(TemplateEffect, this);
    if (NewEffect)
    {
        NewEffect->CurrentStack = 1;
        ActiveCombatEffects.Add(NewEffect);
        UE_LOG(LogTemp, Warning, TEXT("New Effect Added : %s"), *NewEffect->GetName());
    }
    
    /*if (NewEffect)
    {
        ActiveCombatEffects.Add(NewEffect);
    }*/
}

void UDECombatComponent::BroadcastCombatEvent(ECombatEventTrigger TriggerType, FCombatEventData& EventData)
{
    const UEnum* EnumPtr = StaticEnum<ECombatEventTrigger>();
    // 내가 가진 모든 이펙트를 쫙 훑어본다
    for (UDECombatEffect* Effect : ActiveCombatEffects)
    {
        // 유효한지, 그리고 지금 터진 사건(TriggerType)이 이펙트의 명찰과 일치하는지 확인!
        if (Effect && Effect->TriggerCondition == TriggerType)
        {
            Effect->ExecuteEffect(EventData);
            FString EnumName = EnumPtr->GetNameStringByValue((int64)TriggerType);

            //UE_LOG(LogTemp, Error, TEXT("Applied %s_%s in CombatComp"), *EnumName,*Effect->GetName());
        }
    }
}
