// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_GreyDarkness.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_GreyDarkness : public UDEAutoSkillBase
{
	GENERATED_BODY()
	
public:
	UDESkill_GreyDarkness();

	virtual void InitBehaviors() override;

private:

	TSubclassOf<AActor> AOEClass;

};
