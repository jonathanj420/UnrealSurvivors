// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkillVenomCircle.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillVenomCircle : public UDEAutoSkillBase
{
	GENERATED_BODY()
	

    //virtual void ActivateSkill(FDESkillData* SkillData) override;
    virtual void InitBehaviors() override;



private:
    FDESkillData* CurrentData = nullptr;

    TSubclassOf<AActor> AttackClass;
    USoundBase* FireSound;
};
