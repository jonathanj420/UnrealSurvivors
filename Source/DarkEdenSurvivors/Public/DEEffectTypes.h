// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEEffectTypes.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EEffectTrigger : uint8
{
    None        UMETA(Hidden),

    OnKill      UMETA(DisplayName = "On Kill"),
    OnHit       UMETA(DisplayName = "On Hit"),
    OnCrit      UMETA(DisplayName = "On Critical Hit"),
    OnTakeHit   UMETA(DisplayName = "On Take Hit")
};

UENUM(BlueprintType)
enum class EEffectType : uint8
{
    None        UMETA(Hidden),

    // Damage
    Explode,
    StackDamagePercent,

    // Sustain
    HealFixed,
    HealPercent,

    // Economy
    GoldDrop,

    // Crowd Control
    Freeze,
    Slow
};

USTRUCT(BlueprintType)
struct FEffectData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
    EEffectTrigger Trigger = EEffectTrigger::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    EEffectType EffectType = EEffectType::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
    float Value = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Chance = 1.f;

    // Stackable option
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stack")
    bool bIsStackable = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stack", meta = (EditCondition = "bIsStackable"))
    int32 MaxStack = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stack", meta = (EditCondition = "bIsStackable"))
    float StackMultiplier = 1.f;

    // 상태이상 지속 시간 등
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced")
    float Duration = 0.f;
};