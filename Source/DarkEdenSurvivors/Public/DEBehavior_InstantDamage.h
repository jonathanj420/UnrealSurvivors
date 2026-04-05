// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "GameplayTagContainer.h"
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

	// ★ [NEW] 이 레고 블록이 터뜨릴 데미지의 속성과 판정 태그들!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (Categories = "Damage"))
	FGameplayTagContainer SkillDamageTags;

	// 데미지 배율 (첫 타격 2배! 같은 거 할 때 씀)
	UPROPERTY(EditAnywhere)
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere)
	int32 HitCount = 1; // 타격 횟수

	UPROPERTY(EditAnywhere)
	float HitInterval = 0.0f;
};
