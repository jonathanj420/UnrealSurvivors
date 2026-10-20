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
    virtual void InitializeFromContext(const FDESkillContext& Context) override;

    // 초기 각도 설정
    void InitOrbit(float InStartAngle);
    float GetCurrentAngle() const { return CurrentAngle; }

    // 궤도 반경: DT OptionValues "OrbitRadius"(없으면 150) x 범위 스탯
    static float GetOrbitRadius(const FDESkillContext& Context);
    // 회전 속도(도/초): DT OptionValues "OrbitSpeed"(없으면 180) x 투사체 속도 스탯. 음수면 시계 방향
    static float GetOrbitSpeed(const FDESkillContext& Context);

protected:
    float CurrentOrbitRadius = 150.0f;
    float RotationSpeed = 180.0f;
    float CurrentAngle = 0.0f;
};
