// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DECombatEffect.h"
#include "DECombatEffect_Execution.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDECombatEffect_Execution : public UDECombatEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Execution")
    float Threshold = 0.1f; // 10% 이하 즉사

    virtual void OnExecuteEffect(FCombatEventData& EventData) override;
	
};
