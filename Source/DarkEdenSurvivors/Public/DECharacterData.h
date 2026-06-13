// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DEStatComponent.h"
#include "DECharacterData.generated.h"

USTRUCT(BlueprintType)
struct FPassiveMilestone
{
    GENERATED_BODY()

    // 발동 레벨 (시작부터 주려면 1)
    UPROPERTY(EditDefaultsOnly, Category = "Milestone")
    int32 RequiredLevel = 1;

    // 올려줄 스탯 종류 (투사체 개수면 EDEStatType::Amount)
    UPROPERTY(EditDefaultsOnly, Category = "Milestone")
    EDEStatType StatType;

    // 증가량 (+2개면 2.0f)
    UPROPERTY(EditDefaultsOnly, Category = "Milestone")
    float AdditiveBonus = 0.0f;

    // 배율 (이동속도 20% 증가면 1.2f)
    UPROPERTY(EditDefaultsOnly, Category = "Milestone")
    float MultiplierBonus = 1.0f;
};

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDECharacterData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // 캐릭터 고유 레벨업 패시브 트리
    UPROPERTY(EditDefaultsOnly, Category = "Progression")
    TArray<FPassiveMilestone> LevelMilestones;

    // starting skill and mesh +
	
};
