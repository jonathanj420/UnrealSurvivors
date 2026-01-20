// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_SelectNearestTarget.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_SelectNearestTarget : public UDESkillBehavior
{
	GENERATED_BODY()

public:
	virtual void Execute(FDESkillContext& Context) override;

public:
	// 탐색 범위 (Context에 "Range" 값이 있으면 그걸 쓰고, 없으면 이 값을 씀)
	UPROPERTY(EditAnywhere)
	float DefaultSearchRadius = 2000.0f;

	// 적 태그 (예: "Enemy", "Monster")
	UPROPERTY(EditAnywhere)
	FName EnemyTag = TEXT("Enemy");

	
};
