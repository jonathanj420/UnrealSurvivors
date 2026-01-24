// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_BloodyKnife.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_BloodyKnife : public UDEAutoSkillBase
{
	GENERATED_BODY()

public:
    UDESkill_BloodyKnife();
    //virtual void ActivateSkill(FDESkillData* SkillData) override;
    virtual void InitBehaviors() override;

//    void FireRandom();


    int32 ProjectileFired = 0;        // 몇 개 반복했는지
    int32 MaxProjectile = 10;         // 총몇발 연사할지

    FTimerHandle FireTimerHandle;
private:
    FDESkillData* CurrentData = nullptr;

    TSubclassOf<AActor> ProjectileClass;
    USoundBase* FireSound;
};
