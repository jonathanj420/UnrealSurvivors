// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/DESkillData.h"
#include "DESkillBase.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillBase : public UObject
{
	GENERATED_BODY()

public:
	UDESkillBase();

	virtual UWorld* GetWorld() const override;

	virtual void InitSkill(AActor* InOwner);

public:
	UPROPERTY()
	AActor* SkillOwner = nullptr;

	int32 CurrentLevel = 0;
   

};
