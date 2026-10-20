// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_SelectTargetsBySweep.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_SelectTargetsBySweep : public UDESkillBehavior
{
	GENERATED_BODY()
	
public:
	virtual void Execute(FDESkillContext& Context) override;

	// 스윕 길이. -1이면 DT OptionValues의 "Range"(없으면 600), 0 이상이면 이 값
	UPROPERTY(EditAnywhere, Category = "Search")
	float Range = -1.0f;

	// 스윕 반폭. -1이면 Context.Radius(DT), 0 이상이면 이 값
	UPROPERTY(EditAnywhere, Category = "Search")
	float Width = -1.0f;

};
