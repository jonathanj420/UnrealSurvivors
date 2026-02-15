// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEPickupBase.h"
#include "DEPickupEXPGem.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEPickupEXPGem : public ADEPickupBase
{
	GENERATED_BODY()
public:
	ADEPickupEXPGem();

public:
	virtual void ApplyEffect(AActor* TargetActor) override;

};
