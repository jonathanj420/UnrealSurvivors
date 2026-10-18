// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEPickupBase.h"
#include "DEPickup_Gold.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEPickup_Gold : public ADEPickupBase
{
	GENERATED_BODY()
public:
	ADEPickup_Gold();

public:
	virtual void ApplyEffect(AActor* TargetActor) override;

};
