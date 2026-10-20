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
	// 탐색 범위. -1이면 DT OptionValues의 "Radius"(없으면 2000), 0 이상이면 이 값.
	// Context.Radius(장판/투사체 크기)와는 다른 값이라 OptionValues에서 읽는다
	UPROPERTY(EditAnywhere)
	float DefaultSearchRadius = -1.0f;

	
};
