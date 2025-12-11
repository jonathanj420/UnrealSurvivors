// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DEStatusEffectBase.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEStatusEffectBase : public UObject
{
	GENERATED_BODY()
	
    float Duration;
    float Elapsed;
    bool bIsBuff;   // true = buff, false = debuff
    AActor* Target;

    virtual void OnApplied();
    virtual void OnExpired();
    virtual void OnTick(float DeltaTime);
};
