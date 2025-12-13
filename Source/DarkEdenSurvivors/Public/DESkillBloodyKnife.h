// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkillBloodyKnife.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillBloodyKnife : public UDEAutoSkillBase
{
	GENERATED_BODY()

public:
    UDESkillBloodyKnife();
	//virtual void ActivateSkill(const FSkillSpec& Spec) override;
    virtual void ActivateSkill(FDESkillData* SkillData) override;


    //void FireBurst();      // ProjectileCount만큼 동시에 발사
    //void FireSequence();   // 여러 번 반복 발사
    void FireRandom();

    FSkillSpec CurrentSpec;

    int32 ProjectileFired = 0;        // 몇 개 반복했는지
    int32 MaxProjectile = 10;         // 총몇발 연사할지

    FTimerHandle FireTimerHandle;
private:
    FDESkillData* CurrentData = nullptr;

    TSubclassOf<AActor> ProjectileClass;
    USoundBase* FireSound;
};
