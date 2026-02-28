// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_SanguineBats.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_SanguineBats : public UDEAutoSkillBase
{
	GENERATED_BODY()

public:
	UDESkill_SanguineBats();
	//virtual void ActivateSkill(FDESkillData* SkillData) override;
	virtual void InitBehaviors() override;


private:
	FDESkillData* CurrentData = nullptr;

	TSubclassOf<AActor> ProjectileClass;
	USoundBase* FireSound;
	virtual void ExecuteWithContext(FDESkillContext& Context) override;

};
