// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_Delay.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_Delay : public UDESkillBehavior
{
    GENERATED_BODY()

public:
    // 에디터에서 기획자가 조절할 대기 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior|Time")
    float DelayTime = 1.0f;

    // 파이프라인에게 내 딜레이 시간을 알려줌!
    virtual float GetPipelineDelay() const override { return DelayTime; }

    // Execute는 비워두어도 됩니다. (필요시 로그를 찍거나 이펙트를 넣어도 됨)
    virtual void Execute(FDESkillContext& Context) override
    {
        // UI 연출 시작 등을 여기서 할 수도 있습니다.
    }
	
};
