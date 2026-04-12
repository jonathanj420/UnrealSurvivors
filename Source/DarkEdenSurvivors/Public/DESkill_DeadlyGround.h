// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_DeadlyGround.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_DeadlyGround : public UDEAutoSkillBase
{
	GENERATED_BODY()

public:
	UDESkill_DeadlyGround();
	virtual void InitBehaviors() override;


private:

	TSubclassOf<AActor> SummonClass;
	USoundBase* SummonSound;
	
};
