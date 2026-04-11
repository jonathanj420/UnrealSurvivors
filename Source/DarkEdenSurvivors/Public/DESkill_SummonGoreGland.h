// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_SummonGoreGland.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_SummonGoreGland : public UDEAutoSkillBase
{
	GENERATED_BODY()
	
public:
	UDESkill_SummonGoreGland();
	//virtual void ActivateSkill(FDESkillData* SkillData) override;
	virtual void InitBehaviors() override;


private:

	TSubclassOf<AActor> SummonClass;
	USoundBase* SummonSound;

};
