// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkillBloodyNail.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillBloodyNail : public UDEAutoSkillBase
{
	GENERATED_BODY()
    UDESkillBloodyNail();
	virtual void ActivateSkill(FDESkillData* SkillData) override;
    FSkillSpec CurrentSpec;

    FTimerHandle FireTimerHandle;
private:
    FDESkillData* CurrentData = nullptr;

    TSubclassOf<AActor> AttackClass;
    USoundBase* AttackSound;

    bool bRightSide = true;

    void PerformConeSweep();
};
