// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillTypes.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECooldownReduceType : uint8
{
    Flat        UMETA(DisplayName = "고정 시간 감소 (초)"),
    PercentageOfMax         UMETA(DisplayName = "최대 쿨타임 비례 감소 (%)"),
    PercentageOfRemaining   UMETA(DisplayName = "현재 남은 쿨타임 비례 감소 (%)") // ★ 추가됨!
};