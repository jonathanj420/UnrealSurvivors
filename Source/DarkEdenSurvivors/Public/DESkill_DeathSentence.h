// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_DeathSentence.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_DeathSentence : public UDEAutoSkillBase
{
	GENERATED_BODY()

public:
	UDESkill_DeathSentence();
	//virtual void ActivateSkill(FDESkillData* SkillData) override;
	virtual void InitBehaviors() override;


private:

	TSubclassOf<AActor> ProjectileClass;
	USoundBase* FireSound;
	
};
