// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DEStatTypes.generated.h"

/**
 * 게임 내 모든 수치 변경을 관장하는 통합 타입
 * (악세서리, 패시브, 버프, 기본 스탯 등 모두 사용)
 */
UENUM(BlueprintType)
enum class EDEStatType : uint8
{
    // ==========================================
    // [Combat] 전투 관련 (공격, 방어)
    // ==========================================
    Damage          UMETA(DisplayName = "Damage Multiplier"),    // 공격력 %
    CritChance      UMETA(DisplayName = "Crit Chance"),          // 치명타 확률 +
    CritDamage      UMETA(DisplayName = "Crit Damage Multiplier"),          // 치명타 피해 %
    Cooldown        UMETA(DisplayName = "Cooldown Reduction"),   // 쿨타임 감소 %
    Area            UMETA(DisplayName = "Area Size"),            // 범위 %
    Duration        UMETA(DisplayName = "Duration"),             // 지속시간 %
    ProjectileSpeed UMETA(DisplayName = "Projectile Speed"),     // 투사체 속도 %
    Amount          UMETA(DisplayName = "Projectile Amount"),    // 투사체 개수 +
    Knockback       UMETA(DisplayName = "Knockback Force"),      // 넉백 파워 %
    LifeStealChance UMETA(DisplayName = "Life Steal Chance"),      // 피흡 확률 %
    LifeStealMultiplier UMETA(DisplayName = "Life Steal Multiplier"),      // 피흡 증가? %

    // ==========================================
    // [Survival] 생존 및 신체 능력
    // ==========================================
    MaxHP           UMETA(DisplayName = "Max HP"),               // 최대 체력 %
    MoveSpeed       UMETA(DisplayName = "Move Speed"),           // 이동 속도 %
    Armor           UMETA(DisplayName = "Armor"),                // 방어력 (데미지 감소)
    Regeneration        UMETA(DisplayName = "HP Regeneration"),          // 체력 재생 (초당)

    // ==========================================
    // [Utility] 유틸리티 (파밍, 성장)
    // ==========================================
    Magnet          UMETA(DisplayName = "Magnet Range"),         // 자석 범위 %
    Luck            UMETA(DisplayName = "Luck"),                 // 행운 (아이템 드랍 등)
    Greed           UMETA(DisplayName = "Greed (Gold)"),         // 골드 획득량 %
    Growth          UMETA(DisplayName = "Growth (EXP)"),         // 경험치 획득량 %
    Curse           UMETA(DisplayName = "Curse (Enemy Buff)"),   // 저주 (적 강화)
    Revival         UMETA(DisplayName = "Revival Count"),        // 부활 횟수 +

    // ==========================================
    // [Max] 반복문용 (항상 마지막에)
    // ==========================================
    Max             UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FDEStatModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    EDEStatType StatType;

    UPROPERTY(EditAnywhere)
    float Additive = 0.f;

    UPROPERTY(EditAnywhere)
    float Multiplier = 1.f;


    FDEStatModifier() {}
    FDEStatModifier(EDEStatType InType, float InAdd, float InMult)
        : StatType(InType), Additive(InAdd), Multiplier(InMult) {
    }
};

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

    void ApplyModifier(const FDEStatModifier& Mod)
    {
        Additive += Mod.Additive;
        Multiplier *= Mod.Multiplier;
    }

    void RemoveModifier(const FDEStatModifier& Mod)
    {
        Additive -= Mod.Additive;
        Multiplier /= Mod.Multiplier;
    }

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

    float FinalDamageMultiplier = 1.0f; // 기본 배율은 1.0
    float CritChance = 0.0f;            // 치명타 확률 기본 0
    float CritDamageMultiplier = 2.0f;  // 치명타 피해량
    float CooldownReduction = 0.0f;     // 쿨감 기본 0
    float EffectSizeMultiplier = 1.0f;  // 범위 기본 100%
    float DurationMultiplier = 1.0f;    // 지속시간 기본 100%
    int32 BonusAmount = 0;              // 추가 투사체 기본 0개
    float ProjectileSpeedMultiplier = 1.0f; // 속도 기본 100%
    float LifeStealChance = 0.0f;
    float KnockbackMultiplier = 1.0f;

};