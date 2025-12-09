// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DEStageWaveData.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FDEStageWaveData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartTime = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> MonsterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinimumCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnInterval = 0.1f;

    // 보스 또는 특수 몬스터
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<ADEMonsterBase>> Bosses;

    // 맵 이벤트나 트리거용 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString MapEventNotes;
};