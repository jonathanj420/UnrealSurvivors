// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DEStatTypes.h"
#include "DESkillContext.h"
#include "DESkillActorBase.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UStaticMeshComponent;
class UDECombatEffect;

UCLASS()
class DARKEDENSURVIVORS_API ADESkillActorBase : public AActor
{
    GENERATED_BODY()

public:
    ADESkillActorBase();

    // ---------------------------------------------------
    // [생명 주기 (Lifecycle)]
    // 모든 스킬 액터는 이 흐름을 따릅니다.
    // ---------------------------------------------------
    virtual void InitializeFromContext(const FDESkillContext& Context);

protected:
    virtual void ResetState();
    virtual void ReturnToPool();

    UFUNCTION()
    virtual void OnLifeTimeExpired();

    // ---------------------------------------------------
    // [전투 로직 (Combat)]
    // Projectile에 있던 가장 진화된 데미지 로직을 부모가 가집니다.
    // ---------------------------------------------------
    virtual bool TryDealDamage(AActor* Victim);

    // ---------------------------------------------------
    // [컴포넌트 (Components)]
    // ---------------------------------------------------
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UNiagaraComponent* NiagaraComponent;

    // ---------------------------------------------------
    // [공통 데이터 (Stats)]
    // ---------------------------------------------------

    UPROPERTY()
    FDESkillContext CachedContext;

    FCombatSnapshot Snapshot;

    UPROPERTY(EditAnywhere, Instanced, Category = "Local Effects")
    TArray<UDECombatEffect*> LocalEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float KnockbackForce;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float LifeTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Size;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    bool bCanCrit=true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CritChance=0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float CritDamageMultiplier = 1.0f;

    FTimerHandle LifeTimeTimerHandle;

public:

    //*********** GET ***************
    float GetDamage() const { return Damage; }
    float GetKnockbackForce() const { return KnockbackForce; }
    float GetLifeTime() const { return LifeTime; }
    float GetSize() const { return Size; }
    float GetCritChance() const { return CritChance; }
    bool CanCrit() const { return bCanCrit; }
    float GetRadius() const { return Radius; }


    //************** SET ************
    void SetDamage(float v) { Damage = v; }
    void SetKnockbackForce(float v) { KnockbackForce = v; }
    void SetLifeTime(float v) { LifeTime = v; }
    void SetSize(float NewSize);
    void SetCritChance(float v) { CritChance = v; }
    void SetCanCrit(bool v) { bCanCrit = v; }
    void SetRadius(float v) { Radius = v; }

};
