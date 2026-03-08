// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEStatusEffectBase.h"
#include "DEStatusEffect_DotDamage.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEStatusEffect_DotDamage : public UDEStatusEffectBase
{
	GENERATED_BODY()

public:
	// 틱마다 데미지를 입히는 핵심 함수 (C++ 구현부)
	virtual void OnIntervalTick_Implementation() override;

	// 기획 확장성: 독(Poison)은 중첩돼도 딜이 똑같지만, 출혈(Bleed)은 중첩될수록 아프게!
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "StatusEffect|Dot")
	bool bMultiplyByStacks = true;
	
};
