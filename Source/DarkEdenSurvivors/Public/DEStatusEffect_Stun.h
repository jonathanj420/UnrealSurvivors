// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEStatusEffectBase.h"
#include "DEStatusEffect_Stun.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEStatusEffect_Stun : public UDEStatusEffectBase
{
	GENERATED_BODY()

public:
	UDEStatusEffect_Stun();

	virtual void OnApply(AActor* Target, FActiveStatusEffect& EffectData) const override;
	virtual void OnRemove(AActor* Target, FActiveStatusEffect& EffectData) const override;
	
};
