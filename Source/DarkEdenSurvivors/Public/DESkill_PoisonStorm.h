// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_PoisonStorm.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_PoisonStorm : public UDEAutoSkillBase
{
	GENERATED_BODY()
	
	//virtual void ActivateSkill(FDESkillData* SkillData) override;
	virtual void InitBehaviors() override;
};
