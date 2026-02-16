// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatComponent.h"

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
