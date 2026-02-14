// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DEStatTypes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FGameplayStat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Additive; // +10 공격력

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Multiplier; // +10% (1.1)

	FGameplayStat() : BaseValue(0.f), Additive(0.f), Multiplier(1.f) {}
	FGameplayStat(float InBase) : BaseValue(InBase), Additive(0.f), Multiplier(1.f) {}

	float GetValue() const
	{
		// (기본 + 추가) * 배율
		return (BaseValue + Additive) * Multiplier;
	}

	void ResetModifiers()
	{
		Additive = 0.f;
		Multiplier = 1.f;
	}
};

/**
 * 전투 스냅샷 (스킬 발동 시점에 캡처할 데이터)
 * 투사체는 이 데이터만 들고 날아갑니다. (최적화 핵심)
 */
USTRUCT(BlueprintType)
struct FCombatSnapshot
{
	GENERATED_BODY()

	float FinalDamageMultiplier; // 최종 데미지 배율 (플레이어 공격력 등)
	float CritChance;            // 치명타 확률
	float CritDamageMultiplier;  // 치명타 피해량
	float CooldownReduction;     // 쿨타임 감소 (0.0 ~ 1.0)
	float EffectSizeMultiplier;  // 범위 크기 배율
	float DurationMultiplier;    // 지속 시간 배율
	int32 BonusAmount;  // 추가 투사체 개수
	float ProjectileSpeedMultiplier; // 투사체 속도 배율
};