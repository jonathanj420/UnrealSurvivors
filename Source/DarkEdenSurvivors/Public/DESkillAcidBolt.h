// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkillAcidBolt.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillAcidBolt : public UDEAutoSkillBase
{
	GENERATED_BODY()

public:
	UDESkillAcidBolt();
	virtual void ActivateSkill(FDESkillData* SkillData) override;

private:
	FDESkillData* CurrentData = nullptr;

	TSubclassOf<AActor> ProjectileClass;
	USoundBase* FireSound;
	
};
