// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DESaveGame.generated.h"


USTRUCT(BlueprintType)
struct FDEUpgradeData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DamageMultiplierLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HealthMultiplierLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MoveSpeedMultiplierLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BonusAmountLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GreedLevel = 0; // 골드 획득량 증가
};


/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
    UDESaveGame();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
    FString SaveSlotName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SaveData")
    int32 UserIndex;

    // --- 핵심 데이터 ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    int32 TotalGold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
    FDEUpgradeData UpgradeStatus;

};
