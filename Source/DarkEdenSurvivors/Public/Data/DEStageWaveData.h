// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DEStageWaveData.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class EWaveType : uint8
{
    // StartWave에서 MinimumCount만 즉시 스폰하고 끝. 추가 주기 스폰 없음.
    OneShot     UMETA(DisplayName = "One Shot"),

    // Duration 동안 SpawnInterval 주기로 스폰. 시간 끝나면 스폰 중단.
    Timed       UMETA(DisplayName = "Timed"),

    // 다음 웨이브가 시작될 때까지 계속 SpawnInterval 주기로 스폰.
    Infinite    UMETA(DisplayName = "Infinite"),
};

USTRUCT(BlueprintType)
struct FDEStageWaveData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartTime = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> SpawnMonsterIDs;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinimumCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SpawnInterval = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 0.0f;

    // 보스 또는 특수 몬스터
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BossMonsterID;

    // 맵 이벤트나 트리거용 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString MapEventNotes;
};