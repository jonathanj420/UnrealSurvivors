// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESimpleProjectileBase.h"
#include "DEProjectile_Boomerang.generated.h"

UENUM()
enum class EBoomerangPhase : uint8
{
    Going,      // 나가는 중
    Returning   // 돌아오는 중
};

UENUM()
enum class EReturnMode : uint8
{
    Linear,  // 단순 직선 반전
    Homing   // 시전자 유도
};

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEProjectile_Boomerang : public ADESimpleProjectileBase
{

    GENERATED_BODY()
public:
    ADEProjectile_Boomerang();

    // 최대 사거리
    UPROPERTY(EditAnywhere, Category = "Boomerang")
    float MaxDistance = 600.f;

    // 복귀 속도
    UPROPERTY(EditAnywhere, Category = "Boomerang")
    float ReturnSpeed = 900.f;

    // 복귀 방식
    UPROPERTY(EditAnywhere, Category = "Boomerang")
    EReturnMode ReturnMode = EReturnMode::Linear;

    // 시전자 도달 판정 거리
    UPROPERTY(EditAnywhere, Category = "Boomerang")
    float ReturnCompleteDistance = 80.f;

protected:
    virtual void UpdateMovement(float DeltaTime) override;
    virtual void ResetState() override;
    virtual void OnReturnComplete();

    EBoomerangPhase Phase = EBoomerangPhase::Going;
    float TravelDistance = 0.f;
    bool bCanBeReturned = false; //회수 가능?


};
