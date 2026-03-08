// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_Executioner.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_Executioner : public UDEAutoSkillBase
{
	GENERATED_BODY()

public:
	UDESkill_Executioner();
	//virtual void ActivateSkill(FDESkillData* SkillData) override;
	virtual void InitBehaviors() override;


private:

	TSubclassOf<AActor> ProjectileClass;
	USoundBase* FireSound;
	
};
