// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_ApplyStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_ApplyStatusEffect : public UDESkillBehavior
{
	GENERATED_BODY()
public:
	virtual void Execute(FDESkillContext& Context) override;

public:
	// 무슨 상태이상을 걸 것인가? (예: UDEPoisonEffect)
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UDEStatusEffectBase> StatusClass;

	UPROPERTY(EditAnywhere)
	float Duration = 3.0f;

	UPROPERTY(EditAnywhere)
	float Power = 10.f; // 데미지 or 슬로우 비율

	UPROPERTY(EditAnywhere)
	float Interval = 1.0f; // 틱 간격
	
};
