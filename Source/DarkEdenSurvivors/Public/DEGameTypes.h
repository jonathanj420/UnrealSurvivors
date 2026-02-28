// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DEGameTypes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDEStageRow : public FTableRowBase
{
    GENERATED_BODY()

    // 1. 스테이지 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    int32 StageID = 0;

    // 2. 맵 이름 (예: "에슬라니아 시내", "페리고르 탈환전")
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    FText StageName;

    // 3. 맵 설명 (출현 몬스터나 배경 스토리)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    FText Description;

    // 4. 우측에 크게 띄워줄 맵 프리뷰/썸네일 이미지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    class UTexture2D* StageThumbnail = nullptr;

    // ==========================================
    // ★ 5. (가장 중요) 실제로 열어야 할 언리얼 맵(레벨)의 정확한 이름!
    // ==========================================
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    FName LevelName;
};