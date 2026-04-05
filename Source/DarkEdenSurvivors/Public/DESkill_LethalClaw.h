// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_LethalClaw.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_LethalClaw : public UDEAutoSkillBase
{
	GENERATED_BODY()

public:
	UDESkill_LethalClaw();

	//virtual void ActivateSkill(FDESkillData* SkillData) override;
	virtual void InitBehaviors() override;

private:
	// 하드코딩으로 로드한 에셋을 들고 있을 포인터
	UPROPERTY()
	class UNiagaraSystem* CastEffectAsset;

	UPROPERTY()
	class UNiagaraSystem* HitEffectAsset;
	
	UPROPERTY()
	class USoundBase* CastSound;
};
