// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESimpleProjectileBase.h"
#include "DEProjectile_ShadyDoppel.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEProjectile_ShadyDoppel : public ADESimpleProjectileBase
{
    GENERATED_BODY()

public:
    ADEProjectile_ShadyDoppel();

    virtual void InitializeFromContext(const FDESkillContext& Context) override;

protected:
    virtual void UpdateMovement(float DeltaTime) override;

private:
    // [상태 변수]
    bool bIsPiercing;
    float CurrentTime;
    float TotalFlightTime;
    // 진화형(도플2)인지 체크하는 변수! 블루프린트에서 체크박스로 설정 가능
    UPROPERTY(EditDefaultsOnly, Category = "ShadyDoppel")
    bool bCanPierceAfterCurve = false;
    // [베지어 곡선 좌표]
    FVector StartPos;
    FVector ControlPos;
    FVector TargetPos;

    // (옵션) 곡선이 좌우로 얼마나 꺾일지 결정하는 변수
    UPROPERTY(EditDefaultsOnly, Category = "ShadyDoppel")
    float CurveOffsetRange = 500.0f;
    float FlightRate;
	
};
