// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DESkillContext.h"
#include "DEBehavior_FireProjectile.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_FireProjectile : public UDESkillBehavior
{
	GENERATED_BODY()

public:
	virtual void Execute(FDESkillContext& Context) override;

protected:
	// 타이머가 호출할 함수
	void FireOneShot();

public:
	UPROPERTY()
	USoundBase* FireSound;

	UPROPERTY()
	TSubclassOf<AActor> ProjectileClass;

	// random offset,,, 0=zero
	UPROPERTY()
	float RandomPositionRange = 0.f;

	// random angle,,, 0=zero
	UPROPERTY()
	float FireConeAngle = 0.f;

	// burst delay,,, 0=zero
	UPROPERTY()
	float BurstInterval = 0.f;

private:
	// 연사 처리를 위한 임시 저장소
	UPROPERTY()
	FDESkillContext CachedContext;

	int32 RemainingShots = 0;
	FTimerHandle BurstTimerHandle;
	
};
