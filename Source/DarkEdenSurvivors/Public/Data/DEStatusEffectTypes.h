// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEStatusEffectTypes.generated.h"

UENUM(BlueprintType)
enum class EEffectTag : uint8
{
    None      UMETA(DisplayName = "None"),
    Fire      UMETA(DisplayName = "Fire (Burn)"),
    Poison    UMETA(DisplayName = "Poison"),
    Stun      UMETA(DisplayName = "Stun"),
    Slow      UMETA(DisplayName = "Slow"),
    Bleed     UMETA(DisplayName = "Bleed")
};

UENUM(BlueprintType)
enum class EStackPolicy : uint8
{
    Replace   UMETA(DisplayName = "기존 제거 후 덮어쓰기"),
    Refresh   UMETA(DisplayName = "지속시간 초기화"),
    Stack     UMETA(DisplayName = "중첩 허용 (스택 증가)"),
    Ignore    UMETA(DisplayName = "새로운 효과 무시")
};