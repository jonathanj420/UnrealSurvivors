// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_CullByCone.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_CullByCone : public UDESkillBehavior
{
    GENERATED_BODY()

public:
    // 부채꼴 반경 (사거리)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float Radius = 150.0f;

    // 부채꼴 각도 (예: 120도면 좌우 60도씩)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float ConeAngle = 120.0f;

    virtual void Execute(struct FDESkillContext& Context) override;
	
};
