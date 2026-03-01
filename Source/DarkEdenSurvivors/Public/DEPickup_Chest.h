// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEPickupBase.h"
#include "DEPickup_Chest.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEPickup_Chest : public ADEPickupBase
{
	GENERATED_BODY()
	
public:
	ADEPickup_Chest();

	virtual void ApplyEffect(AActor* TargetActor) override;
};
