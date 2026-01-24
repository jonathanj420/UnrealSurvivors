// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_Inferno.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_Inferno : public UDEAutoSkillBase
{
	GENERATED_BODY()

public:
	UDESkill_Inferno();
	virtual void InitBehaviors() override;

private:
	TSubclassOf<AActor> ProjectileClass;
	USoundBase* FireSound;

	
};
