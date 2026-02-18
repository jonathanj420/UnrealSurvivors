// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
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
    TArray<FDEStatModifier> StatModifiers;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TArray<FEffectData> Effects;
};
