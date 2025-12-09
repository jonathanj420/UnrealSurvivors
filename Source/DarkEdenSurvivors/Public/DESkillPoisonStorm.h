// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBase.h"
#include "DESkillPoisonStorm.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillPoisonStorm : public UDESkillBase
{
	GENERATED_BODY()
	
	virtual void ActivateSkill(FDESkillData* SkillData) override;
};
