// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DEEvolutionData.generated.h"

USTRUCT(BlueprintType)
struct FDESkillRequirement
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SkillID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredLevel = 9;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDESkillEvolutionRow : public FTableRowBase
{
    GENERATED_BODY()

public:

    //// 진화 전 스킬
    //UPROPERTY(EditAnywhere, BlueprintReadOnly)
    //int32 BaseSkillID;

    //// 필요 레벨
    //UPROPERTY(EditAnywhere, BlueprintReadOnly)
    //int32 RequiredSkillLevel;

    // ★ BaseSkillID 하나 대신, 필요한 '모든' 스킬과 레벨을 배열로 받습니다!
    // (예: 포이즌(9), 애시드(9), 블러디(9) 세 개를 넣으면 3단 합성 레시피가 됨)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FDESkillRequirement> RequiredSkills;


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