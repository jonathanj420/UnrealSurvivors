// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_DanseMacabre.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_DanseMacabre : public UDEAutoSkillBase
{
    GENERATED_BODY()

public:
    UDESkill_DanseMacabre();

    // 스킬 실행 시 파이프라인을 구성하는 함수
    virtual void InitBehaviors() override;
	
};
