// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatComponent.h"
#include "DECharacterBase.h"

// Sets default values for this component's properties
UDECombatComponent::UDECombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // 스탯 컴포넌트는 틱 필요 없음 (최적화)

	// [기본값 설정] - 기획서에 따라 조정
	DamageMultiplier = FGameplayStat(1.0f);    // 기본 1배
	CritChance = FGameplayStat(0.0f);         // 기본 0%
	CritDamageMultiplier = FGameplayStat(2.0f);          // 기본 200%
	CooldownReduction = FGameplayStat(0.0f);   // 쿨감 0%
	AreaSize = FGameplayStat(1.0f);            // 크기 1배
	Duration = FGameplayStat(1.0f);            // 지속 1배
	Speed = FGameplayStat(1.0f);     // 속도 1배
	BonusAmount = FGameplayStat(0.0f);// 추가 투사체 0개
}

void UDECombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

FCombatSnapshot UDECombatComponent::GetCombatSnapshot() const
{
	FCombatSnapshot Snapshot;

	// 현재 계산된 최종 값들을 구조체에 담습니다.
	Snapshot.FinalDamageMultiplier = DamageMultiplier.GetValue();
	Snapshot.CritChance = CritChance.GetValue();
	Snapshot.CritDamageMultiplier = CritDamageMultiplier.GetValue();

	// 쿨감은 보통 상한선(Cap)이 있습니다. (예: 최대 80%)
	Snapshot.CooldownReduction = FMath::Min(CooldownReduction.GetValue(), 0.8f);

	Snapshot.EffectSizeMultiplier = AreaSize.GetValue();
	Snapshot.DurationMultiplier = Duration.GetValue();
	Snapshot.ProjectileSpeedMultiplier = Speed.GetValue();

	// 투사체 개수는 소수점 버림 (2.5개 -> 2개)
	Snapshot.BonusAmount = FMath::FloorToInt(BonusAmount.GetValue());

	return Snapshot;
}

void UDECombatComponent::HandleDamageDealt(const FDEDamageResult& Result, const FCombatSnapshot& Snapshot)
{
    UE_LOG(LogTemp, Error, TEXT("Try Handle Damage Dealt"));
    // 1. 유효성 체크
    // 데미지가 0이거나, 때린 대상이 없으면 처리 안 함
    if (Result.FinalDamage <= 0.0f || !Result.Victim)
    {
        return;
    }
    UE_LOG(LogTemp, Error, TEXT("HDD Dmg, Victim Passed"));
    // 2. 주인님(Character) 확인
    // 컴포넌트의 주인은 캐릭터여야 힐을 주든 말든 함
    ADECharacterBase* OwnerCharacter = Cast<ADECharacterBase>(GetOwner());
    if (!OwnerCharacter)
    {
        return;
    }
    UE_LOG(LogTemp, Error, TEXT("HDD Owner Passed"));
    // ====================================================
    // [로직 1] 타격 시 발동 효과 (On Hit)
    // ====================================================

    // 생명력 흡수 (Life Steal) 처리
    ProcessLifeSteal(Snapshot);


    // ====================================================
    // [로직 2] 처치 시 발동 효과 (On Kill)
    // ====================================================
    if (Result.bIsDead)
    {
        ProcessOnKillEffect(Result.Victim);
    }
}

void UDECombatComponent::ProcessLifeSteal(const FCombatSnapshot& Snapshot)
{
    // 1. 피흡 확률이 0이면 바로 리턴 (최적화)
    if (Snapshot.LifeStealChance <= 0.0f)
    {
        return;
    }

    // 2. 확률 굴리기 (0.0 ~ 1.0)
    const float Roll = FMath::FRand();

    if (Roll <= Snapshot.LifeStealChance)
    {
        if (ADECharacterBase* OwnerCharacter = Cast<ADECharacterBase>(GetOwner()))
        {
            // [기획 결정 사항]
            // 옵션 A: 고정값 회복 (뱀서류 국룰, 1씩 회복)
            const float HealAmount = 1.0f;

            // 옵션 B: 데미지 비례 회복 (보통 뱀서류에선 너무 강력해서 잘 안 씀)
            // const float HealAmount = Result.FinalDamageApplied * 0.1f; 

            OwnerCharacter->Heal(HealAmount);

            // (선택) 로그 및 이펙트
            // UE_LOG(LogTemp, Log, TEXT("Blood Sucked! Healed: %f"), HealAmount);
            // OwnerCharacter->PlayFX("VampireEffect"); 
        }
    }
}

void UDECombatComponent::ProcessOnKillEffect(AActor* Victim)
{
    ADECharacterBase* OwnerCharacter = Cast<ADECharacterBase>(GetOwner());
    if (!OwnerCharacter) return;

    UE_LOG(LogTemp, Log, TEXT("On Kill Effect Test"));
    // 1. 킬 카운트 증가 (게임 모드나 캐릭터에 저장)
    // OwnerCharacter->AddKillCount(1);

    // 2. (확장) 처치 시 쿨타임 감소?
    // if (OwnerCharacter->HasPassive("ResetOnKill")) { ... }

    // 3. (확장) 처치 시 시체 폭발? (유물 효과 등)
    // UGameplayStatics::SpawnEmitterAtLocation(..., Victim->GetActorLocation(), ...);

    // 로그 확인용
    // UE_LOG(LogTemp, Verbose, TEXT("Enemy Killed by %s"), *GetName());
}
