// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "DESkillActorBase.h"
#include "DESimpleProjectileBase.generated.h"

UCLASS()
class DARKEDENSURVIVORS_API ADESimpleProjectileBase : public ADESkillActorBase
{
    GENERATED_BODY()

public:
    ADESimpleProjectileBase();
    virtual void Tick(float DeltaTime) override;

    virtual void InitializeFromContext(const FDESkillContext& Context) override;

protected:

    virtual void ResetState() override;
    virtual void ReturnToPool() override;
    virtual void UpdateMovement(float DeltaTime);

    virtual void PerformCollisionDetection(float DeltaTime) override;

    /*UFUNCTION()
    virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);*/

    // --- 투사체 전용 컴포넌트 ---
    UPROPERTY(VisibleAnywhere)
    class UProjectileMovementComponent* MovementComponent;

    UPROPERTY(VisibleAnywhere)
    class URotatingMovementComponent* RotatingMovementComponent;

    // --- 투사체 전용 변수 ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Stat")
    int32 Penetration = 1;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Movement")
    float Speed = 1200.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Movement")
    float Acceleration = 0.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Movement")
    float CurrentSpeed = 0.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Movement")
    FVector ShootDirection;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Movement")
    bool bEnableRotation = false;
    // 회전 속도 (bEnableRotation이 true일 때만 에디터에서 활성화됨!)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Movement", meta = (EditCondition = "bEnableRotation"))
    FRotator CustomRotationRate = FRotator(0.f, 720.f, 0.f);

    TSet<AActor*> HitActors;

    FVector LastFrameLocation;

public:
    void SetSpeed(float NewSpeed);

};
