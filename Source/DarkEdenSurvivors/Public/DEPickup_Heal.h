// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEPickupBase.h"
#include "DEPickup_Heal.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEPickup_Heal : public ADEPickupBase
{
	GENERATED_BODY()
public:
	ADEPickup_Heal();
	virtual void ApplyEffect(AActor* TargetActor) override;
	
};
