// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_FireProjectileRadial.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_FireProjectileRadial : public UDESkillBehavior
{
	GENERATED_BODY()

public:
	virtual void Execute(FDESkillContext& Context) override;

public:
	UPROPERTY()
	USoundBase* FireSound;

	UPROPERTY()
	TSubclassOf<AActor> ProjectileClass;

	// (옵션) 360도 전체가 아니라 180도만 쏘고 싶을 때를 대비해 변수화 가능
	// 기본값 360.0f
	UPROPERTY()
	float ArcAngle = 360.f;
};
