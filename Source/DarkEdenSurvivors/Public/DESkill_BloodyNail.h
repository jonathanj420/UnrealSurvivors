// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_BloodyNail.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_BloodyNail : public UDEAutoSkillBase
{
	GENERATED_BODY()
    UDESkill_BloodyNail();
	//virtual void ActivateSkill(FDESkillData* SkillData) override;
    virtual void InitBehaviors() override;

    FTimerHandle FireTimerHandle;
private:
    FDESkillData* CurrentData = nullptr;

    TSubclassOf<AActor> AttackClass;
    USoundBase* AttackSound;

    bool bRightSide = true;

    void PerformConeSweep();
};
