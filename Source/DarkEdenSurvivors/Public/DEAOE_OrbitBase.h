// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESimpleAOEBase.h"
#include "DEAOE_OrbitBase.generated.h"


class UStaticMeshComponent;
struct FDESkillContext;
/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEAOE_OrbitBase : public ADESimpleAOEBase
{
    GENERATED_BODY()

public:
    ADEAOE_OrbitBase();

    virtual void Tick(float DeltaTime) override;
    //virtual void ApplyContext(const FDESkillContext& Context) override;

    // 초기 각도 설정
    void InitOrbit(float InStartAngle);

protected:
    float CurrentOrbitRadius = 150.0f;
    float RotationSpeed = 180.0f;
    float CurrentAngle = 0.0f;
    float MaxRadius = 400.0f;
};
