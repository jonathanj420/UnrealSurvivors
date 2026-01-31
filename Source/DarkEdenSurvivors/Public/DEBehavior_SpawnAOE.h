// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_SpawnAOE.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_SpawnAOE : public UDESkillBehavior
{
	GENERATED_BODY()
	
public:
	virtual void Execute(FDESkillContext& Context) override;
};
