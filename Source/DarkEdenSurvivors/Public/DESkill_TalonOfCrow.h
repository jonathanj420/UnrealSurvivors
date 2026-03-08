// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_TalonOfCrow.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_TalonOfCrow : public UDEAutoSkillBase
{
	GENERATED_BODY()
	
	UDESkill_TalonOfCrow();

	virtual void InitBehaviors() override;
	virtual void ExecuteWithContext(FDESkillContext& Context) override;

protected:
	UPROPERTY()
	class UNiagaraSystem* NailHitEffect;

	UPROPERTY()
	class USoundBase* NailHitSound;

};
