// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEDamageTypes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDamageVisualInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    float Amount = 0.f;

    UPROPERTY(BlueprintReadOnly)
    FVector WorldLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    bool bIsCritical = false;
};