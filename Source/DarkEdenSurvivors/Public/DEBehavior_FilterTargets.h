// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_FilterTargets.generated.h"

// 필터링 기준
UENUM()
enum class ETargetFilterType : uint8
{
	Random,         
	Nearest,        
	Farthest,       
	LowestHP,       
	HighestHP       
};

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_FilterTargets : public UDESkillBehavior
{
	GENERATED_BODY()

public:
	UDEBehavior_FilterTargets();
	virtual void Execute(FDESkillContext& Context) override;

public:
	// 남길 타겟 수 (예: 1이면 1마리만, 5면 5마리만 남김)
	UPROPERTY(EditAnywhere, Category = "Filter")
	int32 TargetCount = 1;

	// 필터링 방식
	UPROPERTY(EditAnywhere, Category = "Filter")
	ETargetFilterType FilterType = ETargetFilterType::Random;
	
};
