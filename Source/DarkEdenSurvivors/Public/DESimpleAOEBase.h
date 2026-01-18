// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DESimpleAOEBase.generated.h"

UCLASS()
class DARKEDENSURVIVORS_API ADESimpleAOEBase : public AActor
{
    GENERATED_BODY()

public:
    ADESimpleAOEBase();

    void InitializeAOE(
        float InDamage,
        float InRadius,
        float InTickInterval,
        int32 InMaxTickCount,
        float InLifeTime,
        AActor* InOwner
    );

protected:
    virtual void BeginPlay() override;

    void ApplyDamageOnce();
    void TickDamage();
    void FinishAOE();
    void ReturnToPool();

protected:
    UPROPERTY(VisibleAnywhere)
    class USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere)
    class UNiagaraComponent* NiagaraComp;

    float Damage = 0.f;
    float TickInterval = 0.f;
    int32 MaxTickCount = 0;
    int32 CurrentTickCount = 0;
    float LifeTime = 0.f;

    TWeakObjectPtr<AActor> OwnerActor;

    FTimerHandle TickTimerHandle;

};
