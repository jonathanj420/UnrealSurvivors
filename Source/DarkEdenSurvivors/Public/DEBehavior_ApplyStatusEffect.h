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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect")
	TSubclassOf<class UDEStatusEffectBase> StatusClass;

	// -1.0 이면 Context(스킬 최종 스펙)의 Duration을 따라가고, 
	// 양수이면 이 블루프린트에 적힌 고정값을 우선적으로 씁니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect")
	float Duration = -1.0f;

	// -1.0 이면 Context의 Power를 따라갑니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect")
	float Power = -1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect")
	float Interval = 1.0f;
	
};
