// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkillBloodySpear.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillBloodySpear : public UDEAutoSkillBase
{
	GENERATED_BODY()
	
	UDESkillBloodySpear();

	//virtual void ActivateSkill(FDESkillData* SkillData) override;
	virtual void InitBehaviors() override;
	class ADEMonsterBase* FindBestTarget();
};
