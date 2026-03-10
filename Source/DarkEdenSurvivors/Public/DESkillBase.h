// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
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
	void SetSkillID(int32 InSkillID) { SkillID = InSkillID; }
	int32 GetSkillID() { return SkillID; }
protected:
	UPROPERTY()
	AActor* SkillOwner = nullptr;

	UPROPERTY()
	class UDECombatComponent* CachedCombatComp;

	int32 SkillID = -1;
	int32 CurrentLevel = 0;
   

};
