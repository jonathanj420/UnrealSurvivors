// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEPickupBase.h"
#include "DEPickup_Magnet.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEPickup_Magnet : public ADEPickupBase
{
	GENERATED_BODY()

public:
	ADEPickup_Magnet();

protected:
	// 플레이어가 먹었을 때 실행될 함수
	virtual void ApplyEffect(AActor* TargetActor) override;
	
};
