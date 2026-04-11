// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEProjectile_SanguineBat.h"
#include "DEProjectile_CrimsonHarvest.generated.h"

UENUM(BlueprintType)
enum class EBatPhase : uint8
{
	Hunting,  // 적들 사이를 튕겨다니는 기본 상태
	Returning // 수명이 다해 플레이어에게 돌아가는 상태
};

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEProjectile_CrimsonHarvest : public ADEProjectile_SanguineBat
{
    GENERATED_BODY()

public:
    ADEProjectile_CrimsonHarvest();

protected:
    virtual void ResetState() override;
    virtual void OnLifeTimeExpired() override;
    virtual void UpdateMovement(float DeltaTime) override;
    virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Phase")
    EBatPhase CurrentPhase = EBatPhase::Hunting;

    // 돌아올 때의 속도와 회전(호밍) 민첩성
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float ReturnSpeed = 1500.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float HomingTurnSpeed = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "HPSteal")
    float HealAmount = 1.0f;
    bool bHasEverDealt = false;
	
};
