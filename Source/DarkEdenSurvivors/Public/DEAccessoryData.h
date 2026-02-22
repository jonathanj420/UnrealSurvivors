// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "DEEffectTypes.h"
#include "DEStatTypes.h"
#include "DEAccessoryData.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEAccessoryData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    int32 ID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    UTexture2D* Icon = nullptr;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    int32 MaxLevel = 5;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    TArray<FDEStatModifier> StatModifiers;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TArray<FEffectData> Effects;
};

USTRUCT(BlueprintType)
struct FDEAccessoryRow : public FTableRowBase
{
    GENERATED_BODY()

    // 어떤 악세서리인가? (위에서 만든 데이터 에셋을 쏙 넣음)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UDEAccessoryData* AccessoryData = nullptr;

    // 등장 확률 가중치 (기본 100. 전설템은 10~20으로 낮추면 됨!)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DropWeight = 100.0f;
};
