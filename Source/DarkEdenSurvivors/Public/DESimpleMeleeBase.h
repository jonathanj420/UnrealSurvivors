// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "DESimpleMeleeBase.generated.h"

UCLASS()
class DARKEDENSURVIVORS_API ADESimpleMeleeBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADESimpleMeleeBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    //************** Components ************
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionComponent;

    // 실제 Niagara 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    UNiagaraComponent* NiagaraEffectComponent;

    // 나이아가라 시스템 에셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    UNiagaraSystem* NiagaraSystem;

    // ***************** Projectile Stats ******************
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float KnockbackForce;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float SweepRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float BaseRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CritChance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    bool bCanCrit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    bool bIsCrit;


    // 스킬 스펙으로 초기화
protected:

    void ResetState();

    /** 풀로 반환 */
    UFUNCTION()
    void OnNiagaraFinished(UNiagaraComponent* PSystem);
    void ReturnToPool();

public:
    // ******** Sweep 공격 ********
    UFUNCTION()
    virtual void PerformSweepAttack();

    TSet<AActor*> HitActors;
    //*********** GET ***************
    float GetDamage() { return Damage; }
    float GetKnockbackForce() { return KnockbackForce; }
    float GetSweepRadius() { return SweepRadius; }
    float GetCritChance() { return CritChance; }
    bool CanCrit() { return bCanCrit; }
    bool IsCrit() { return bIsCrit; }


    //************** SET ************
    void SetDamage(float v) { Damage = v; }
    void SetKnockbackForce(float v) { KnockbackForce = v; }
    void SetSweepRadius(float v) { SweepRadius = v; }
    void SetCritChance(float v) { CritChance = v; }
    void SetCanCrit(bool v) { bCanCrit = v; }
    void SetIsCrit(bool v) { bIsCrit = v; }
};
