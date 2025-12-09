// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESimpleMeleeBase.h"
#include "DEBloodyNail.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEBloodyNail : public ADESimpleMeleeBase
{
	GENERATED_BODY()

public:

	ADEBloodyNail();

	virtual void PerformSweepAttack() override;
	void DoFanShapeAttack(float Damage, float Range, float AngleDegrees);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	class ADEMonsterSpawnManager* MonsterManager;
	
};
