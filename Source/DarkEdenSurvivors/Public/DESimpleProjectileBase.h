// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/Actor.h"
#include "DESimpleProjectileBase.generated.h"

UCLASS()
class DARKEDENSURVIVORS_API ADESimpleProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADESimpleProjectileBase();

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

    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere)
    class UProjectileMovementComponent* MovementComponent;


    // ***************** Projectile Stats ******************
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Stats")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    int32 Penetration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float KnockbackForce;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float LifeTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float LifeTimeCounter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Speed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float Size;

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
    void ReturnToPool();

public:
    virtual void InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction);
    float LifeTimer = 0.f;
    virtual void OnLifeTimeExpired();

    UFUNCTION()
    virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    TSet<AActor*> HitActors;
    //*********** GET ***************
    float GetDamage() { return Damage; }
    int32 GetPenetration() { return Penetration; }
    float GetKnockbackForce() { return KnockbackForce; }
    float GetLifeTime() { return LifeTime; }
    float GetSpeed() { return Speed; }
    float GetSize() { return Size; }
    float GetCritChance() { return CritChance; }
    bool CanCrit() { return bCanCrit; }
    bool IsCrit() { return bIsCrit; }


    //************** SET ************
    void SetDamage(float v) { Damage = v; }
    void SetPenetration(int32 v) { Penetration = v; }
    void SetKnockbackForce(float v) { KnockbackForce = v; }
    void SetLifeTime(float v) { LifeTime = v; }
    void SetSpeed(float v) { Speed = v; }
    void SetSize(float v) { Size = v; }
    void SetCritChance(float v) { CritChance = v; }
    void SetCanCrit(bool v) { bCanCrit = v; }
    void SetIsCrit(bool v) { bIsCrit = v; }







};
