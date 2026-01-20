// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_InstantDamage.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_InstantDamage : public UDESkillBehavior
{
	GENERATED_BODY()
public:
	virtual void Execute(FDESkillContext& Context) override;

	// 데미지 배율 (첫 타격 2배! 같은 거 할 때 씀)
	UPROPERTY(EditAnywhere)
	float DamageMultiplier = 1.0f;
};
