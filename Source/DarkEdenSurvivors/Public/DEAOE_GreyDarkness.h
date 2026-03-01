// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESimpleAOEBase.h"
#include "DEAOE_GreyDarkness.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEAOE_GreyDarkness : public ADESimpleAOEBase
{
	GENERATED_BODY()

public:
	ADEAOE_GreyDarkness();

protected:
	/* ===================== Garlic Specific ===================== */

	/** 히트 시 추가 Knockback */
	UPROPERTY(EditDefaultsOnly, Category = "Darkness")
	float KnockbackPerHit = 0.3f;

	/** 히트 시 Freeze 저항 감소 */
	UPROPERTY(EditDefaultsOnly, Category = "Darkness")
	float FreezeResistReduction = 0.1f;

	/** 최대 Knockback 누적 */
	UPROPERTY(EditDefaultsOnly, Category = "Darkness")
	float MaxAdditionalKnockback = 2.0f;

	/* ===================== Runtime ===================== */

	/** Enemy별 누적 Knockback */
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, float> AccumulatedKnockback;

	/** Enemy별 Freeze 저항 */
	UPROPERTY()
	TMap<TWeakObjectPtr<AActor>, float> CurrentFreezeResist;

	/* ===================== Overrides ===================== */

	virtual void OnHitTarget(AActor* Target) override;

	/* ===================== Helpers ===================== */

	void ApplyDarknessEffects(AActor* Target);
	
};
