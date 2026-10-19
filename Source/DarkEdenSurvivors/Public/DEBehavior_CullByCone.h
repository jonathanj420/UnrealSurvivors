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

    // 부채꼴 각도 (예: 120도면 좌우 60도씩). DT OptionValues에 "Angle"이 있으면 그 값이 우선한다.
    // 기본값 60은 이 프로퍼티가 안 읽히던 시절의 하드코딩 값과 같다 (그대로 둔 BP의 동작 유지)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float ConeAngle = 60.0f;

    virtual void Execute(struct FDESkillContext& Context) override;
	
};
