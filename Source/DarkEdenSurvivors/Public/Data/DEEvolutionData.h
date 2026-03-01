// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DEEvolutionData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDESkillEvolutionRow : public FTableRowBase
{
    GENERATED_BODY()

public:

    // 진화 전 스킬
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BaseSkillID;

    // 필요 레벨
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RequiredSkillLevel;

    // 필요 악세서리들
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<int32> RequiredAccessoryIDs;

    // 진화 결과 스킬
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 ResultSkillID;

    // 여러 진화 충돌 시 우선순위
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Priority = 0;
};