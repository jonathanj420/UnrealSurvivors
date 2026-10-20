// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_BloodDrain.h"

#include "DECharacterBase.h"
#include "DEMonsterBase.h"
#include "DEMonsterSpawnManager.h"

#include "DEGameMode_Stage.h"
#include "DEGameplayLibrary.h"
#include "DEStatusEffectComponent.h"
#include "DEStatusEffect_Stun.h"
#include "DESkillContext.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

UDESkill_BloodDrain::UDESkill_BloodDrain()
{
    StunEffectClass = UDEStatusEffect_Stun::StaticClass();

    static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
        NS_BloodDrain(TEXT("/Game/DarkEden/Data/Niagara/NS_BloodDrainCustom.NS_BloodDrainCustom"));

    if (NS_BloodDrain.Succeeded())
    {
        BloodDrainEffect = NS_BloodDrain.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Vampire_Misc_BD.Vampire_Misc_BD")
    );

    if (SoundObj.Succeeded())
    {
        BloodDrainSound = SoundObj.Object;
    }
}

void UDESkill_BloodDrain::ActivateSkill()
{
    ADECharacterBase* Player = Cast<ADECharacterBase>(SkillOwner);
    if (!Player)
        return;

    if (!Player->CanActivateBloodDrain())
        return;

    Player->ConsumeBloodDrainGauge();

    // 이동 / 자동공격 정지
    Player->SetCanMove(false);
    Player->SetAutoSkillsPaused(true);

    // 디버그
    DrawDebugSphere(
        GetWorld(),
        Player->GetActorLocation(),
        DrainRadius,
        16,
        FColor::Red,
        false,
        1.0f
    );

    StartBloodDrain(Player);

    

    // 종료 타이머
    GetWorld()->GetTimerManager().SetTimer(
        BloodDrainTimerHandle,
        this,
        &UDESkill_BloodDrain::FinishBloodDrain,
        StunDuration,
        false
    );
}


void UDESkill_BloodDrain::StartBloodDrain(ADECharacterBase* Player)
{
    CachedTargets.Empty();

    TArray<ADEMonsterBase*> Targets;
    CollectTargets(Targets);

    if (Targets.Num() == 0)
    {
        return;

    }
    if (BloodDrainSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            Player,
            BloodDrainSound,
            Player->GetActorLocation()
        );
    }


    TArray<FVector> MonsterPositions;

    for (ADEMonsterBase* Monster : Targets)
    {
        if (!Monster || !Monster->IsAlive())
            continue;


        CachedTargets.Emplace(Monster, Monster->GetSpawnSerial());
        MonsterPositions.Add(Monster->GetActorLocation());
        if (Monster->StatusEffectComponent)
        {
            Monster->StatusEffectComponent->AddEffect(StunEffectClass, Player, StunDuration, 0.f, 0.f, FDESkillContext());
        }

        
    }
    SpawnBloodDrainNiagara(Player, MonsterPositions);
}

void UDESkill_BloodDrain::FinishBloodDrain()
{
    ADECharacterBase* Player = Cast<ADECharacterBase>(SkillOwner);
    if (!Player)
        return;

    float TotalHeal = 0.f;

    for (const TPair<TWeakObjectPtr<ADEMonsterBase>, uint32>& Target : CachedTargets)
    {
        ADEMonsterBase* Monster = Target.Key.Get();
        if (!Monster || !Monster->IsAlive() || Monster->GetSpawnSerial() != Target.Value)
            continue;

        ApplyFinishDamage(Monster, Player);
        TotalHeal += DrainAmountPerMonster;
    }

    Player->Heal(TotalHeal);

    // 이동 / 자동공격 복구
    Player->SetCanMove(true);
    Player->SetAutoSkillsPaused(false);

    CachedTargets.Empty();
}

void UDESkill_BloodDrain::CollectTargets(
    TArray<ADEMonsterBase*>& OutTargets) const
{
    const ADECharacterBase* Player = Cast<ADECharacterBase>(SkillOwner);
    if (!Player)
        return;

    const FVector PlayerLocation = Player->GetActorLocation();
    const float RadiusSq = DrainRadius * DrainRadius;

    ADEGameMode_Stage* GameMode = Player->GetWorld()->GetAuthGameMode<ADEGameMode_Stage>();
    if (!GameMode)
        return;

    ADEMonsterSpawnManager* SpawnManager =
        GameMode->GetMonsterSpawnManager();
    if (!SpawnManager)
        return;

    const TArray<ADEMonsterBase*>& Monsters =
        SpawnManager->GetActiveMonsters();

    for (ADEMonsterBase* Monster : Monsters)
    {
        if (!Monster || !Monster->IsAlive())
            continue;

        // 보스는 항상, 그 외는 DT 행 설정에 따라 광흡 면역
        if (Monster->IsImmuneToBloodDrain())
            continue;

        const float DistSq = FVector::DistSquared(
            PlayerLocation,
            Monster->GetActorLocation()
        );

        if (DistSq <= RadiusSq)
        {
            OutTargets.Add(Monster);
        }
    }
}

void UDESkill_BloodDrain::SpawnBloodDrainNiagara(
    ADECharacterBase* Player,
    const TArray<FVector>& MonsterPositions
) const
{
    if (!BloodDrainEffect)
        return;

    UNiagaraComponent* NiagaraComp =
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            Player->GetWorld(),
            BloodDrainEffect,
            Player->GetActorLocation()
        );

    if (!NiagaraComp)
        return;

    // 플레이어 위치
    NiagaraComp->SetVectorParameter(
        TEXT("PlayerPosition"),
        Player->GetActorLocation()
    );

    // 시전 시간 (초). 흐름이 플레이어에게 모이는 시점을 마무리 데미지 시점에 맞추는 데 쓴다
    NiagaraComp->SetFloatParameter(TEXT("DrainDuration"), StunDuration);

    //// ⭐ 몬스터 위치 배열 전달
    //NiagaraComp->SetNiagaraVariableVec3Array(
    //    TEXT("User.MonsterPositions"),
    //    MonsterPositions
    //);
    //NiagaraComp->setvariablevec3array
    UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
        NiagaraComp,
        TEXT("MonsterPosi"), // 나이아가라 User Parameter 이름과 일치
        MonsterPositions);
}

void UDESkill_BloodDrain::ApplyFinishDamage(
    ADEMonsterBase* Monster,
    ADECharacterBase* Player
) const
{
    // ApplyCombatDamage를 거쳐야 킬 집계, OnHit/OnKill, 킬러 게이지가 붙는다. 크리티컬은 없다 (CritChance 0)
    FDEDamageRequest Req;
    Req.Instigator = Player;
    Req.DamageCauser = Player;
    Req.SourceObject = const_cast<UDESkill_BloodDrain*>(this);
    Req.Victim = Monster;
    Req.BaseDamage = Monster->GetMaxHP() * DrainDamagePercent;

    UDEGameplayLibrary::ApplyCombatDamage(Req);
}