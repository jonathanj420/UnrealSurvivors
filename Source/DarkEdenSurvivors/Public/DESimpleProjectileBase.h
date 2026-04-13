// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
//#include "GameFramework/Actor.h"
//#include "DEStatTypes.h"
//#include "DESkillContext.h"
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

    UFUNCTION()
    virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

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

public:
    void SetSpeed(float NewSpeed);
//	GENERATED_BODY()
//	
//public:	
//	// Sets default values for this actor's properties
//	ADESimpleProjectileBase();
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//    //************** Components ************
//protected:
//    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
//    USphereComponent* CollisionComponent;
//
//    UPROPERTY(VisibleAnywhere)
//    class UStaticMeshComponent* Mesh;
//
//    UPROPERTY(VisibleAnywhere)
//    class UProjectileMovementComponent* MovementComponent;
//
//    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
//    class URotatingMovementComponent* RotatingMovementComponent;
//
//    // 회전 기능을 사용할지 여부 (기본값: false)
//    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Movement")
//    bool bEnableRotation = false;
//
//    // 회전 속도 (bEnableRotation이 true일 때만 에디터에서 활성화됨!)
//    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Movement", meta = (EditCondition = "bEnableRotation"))
//    FRotator CustomRotationRate = FRotator(0.f, 720.f, 0.f);
//
//
//    // ***************** Projectile Stats ******************
//protected:
//    UPROPERTY()
//    FDESkillContext CachedContext;
//
//    FCombatSnapshot Snapshot;
//protected:
//    // 이 투사체'만' 가지고 있는 고유 특수 효과들
//    UPROPERTY(EditAnywhere, Instanced, Category = "Local Effects")
//    TArray<class UDECombatEffect*> LocalEffects;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Stats")
//    float Damage;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    int32 Penetration;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    float KnockbackForce;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    float LifeTime;
//
//    FTimerHandle LifeTimeTimerHandle;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    float Speed;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    float Size;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    float CritChance;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    float CritDamageMultiplier = 1.0f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    bool bCanCrit;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    bool bIsCrit;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    float EffectRadius;
//
//    // protected 컴포넌트 섹션에 추가
//    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
//    TObjectPtr<UNiagaraComponent> NiagaraComponent;
//
//    // 초당 속도 변화량 (양수면 가속, 음수면 감속)
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    float Acceleration = 0.0f;
//
//    // 방향을 저장할 변수 추가 (단위 벡터)
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    FVector ShootDirection;
//
//    // 현재 속력 저장 (매번 Velocity.Size()로 계산하지 않게)
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
//    float CurrentSpeed;
//
//    // 스킬 스펙으로 초기화
//    
//protected:
//    virtual void ResetState();
//
//    /** 풀로 반환 */
//    void ReturnToPool();
//
//public:
//    virtual void InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction);
//    virtual void InitializeFromContext(const FDESkillContext& Context, const FVector& Direction);
//    virtual void OnLifeTimeExpired();
//
//    UFUNCTION()
//    virtual void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
//        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
//        bool bFromSweep, const FHitResult& SweepResult);
//
//protected:
//    bool TryDealDamage(AActor* Victim);
//    virtual void UpdateMovement(float DeltaTime);
//
//    TSet<AActor*> HitActors;
//
//



};
