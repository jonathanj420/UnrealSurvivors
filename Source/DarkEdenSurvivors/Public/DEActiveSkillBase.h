// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBase.h"
#include "DEActiveSkillBase.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEActiveSkillBase : public UDESkillBase
{
	GENERATED_BODY()
public:
    UDEActiveSkillBase();

    /** 발동 가능 여부 (게이지, 쿨타임 등) */
    virtual bool CanActivate() const PURE_VIRTUAL(UDEActiveSkillBase::CanActivate, return false;);

    /** 실제 스킬 발동 */
    virtual void ActivateSkill() PURE_VIRTUAL(UDEActiveSkillBase::ActivateSkill, );

protected:
    /** 공통 쿨다운 */
    float Cooldown = 0.f;
    float LastActivateTime = -FLT_MAX;

};
