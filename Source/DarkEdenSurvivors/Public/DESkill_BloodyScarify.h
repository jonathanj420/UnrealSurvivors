// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_BloodyScarify.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_BloodyScarify : public UDEAutoSkillBase
{
	GENERATED_BODY()

	UDESkill_BloodyScarify();

	//virtual void ActivateSkill(FDESkillData* SkillData) override;
	virtual void InitBehaviors() override;

private:
	UPROPERTY()
	class UNiagaraSystem* NiagaraFx;

	UPROPERTY()
	class USoundBase* SoundFx;

	UPROPERTY()
	TSubclassOf<class UDEStatusEffectBase> StatusEffectClass;
	
};
