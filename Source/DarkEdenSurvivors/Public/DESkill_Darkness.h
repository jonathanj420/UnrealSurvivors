// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "NiagaraSystem.h"
#include "DESkill_Darkness.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_Darkness : public UDEAutoSkillBase
{
	GENERATED_BODY()
public:
	UDESkill_Darkness();

	virtual void InitBehaviors() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UNiagaraSystem> CastEffectAsset;

	UPROPERTY(Transient)
	TObjectPtr<UNiagaraSystem> HitEffectAsset;


};
