// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_FireProjectileRadial.generated.h"

UENUM(BlueprintType)
enum class EProjectileFireMode : uint8
{
	Random      UMETA(DisplayName = "Random"),
	Sequential  UMETA(DisplayName = "Sequential")
};

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
	UPROPERTY(EditAnywhere)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TArray<TSubclassOf<class ADESimpleProjectileBase>> ProjectileClasses;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	EProjectileFireMode FireMode = EProjectileFireMode::Random;

	int32 CurrentFireIndex = 0;

	// (옵션) 360도 전체가 아니라 180도만 쏘고 싶을 때를 대비해 변수화 가능
	// 기본값 360.0f
	UPROPERTY(EditAnywhere)
	float ArcAngle = 360.f;
};
