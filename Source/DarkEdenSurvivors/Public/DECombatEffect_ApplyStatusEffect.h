// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DECombatEffect.h"
#include "DECombatEffect_ApplyStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDECombatEffect_ApplyStatusEffect : public UDECombatEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "StatusEffect")
    TSubclassOf<class UDEStatusEffectBase> StatusEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect")
    float Duration = 3.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect")
    float Interval = 1.0f;

    virtual void OnExecuteEffect(FCombatEventData& EventData) override;
};
