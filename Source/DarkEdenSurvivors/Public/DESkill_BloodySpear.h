// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_BloodySpear.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_BloodySpear : public UDEAutoSkillBase
{
	GENERATED_BODY()
	
	UDESkill_BloodySpear();

	//virtual void ActivateSkill(FDESkillData* SkillData) override;
	virtual void InitBehaviors() override;
	class ADEMonsterBase* FindBestTarget();
};
