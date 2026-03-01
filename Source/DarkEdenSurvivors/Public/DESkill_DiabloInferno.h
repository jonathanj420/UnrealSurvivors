// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_DiabloInferno.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_DiabloInferno : public UDEAutoSkillBase
{
	GENERATED_BODY()

public:
	UDESkill_DiabloInferno();
	virtual void InitBehaviors() override;
	virtual void Activate() override;

private:
	TSubclassOf<AActor> ProjectileClass;
	USoundBase* FireSound;
};
