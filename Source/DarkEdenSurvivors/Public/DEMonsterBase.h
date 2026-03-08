// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/Actor.h"
#include "DEMonsterBase.generated.h"

//class ADEMonsterBase;
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDeath, ADEMonsterBase*, Monster);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDeath, ADEMonsterBase*, Monster);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMonsterDeath, ADEMonsterBase*);

enum class EMonsterCrowdControl : uint8
{
	None,
	Stun,
	// Slow,
	// Root,
	// Fear ...
};

class UDEStatComponent;
class UDEHealthComponent;

UCLASS()
class DARKEDENSURVIVORS_API ADEMonsterBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADEMonsterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//*************** COMPONENTS ******************
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDEStatComponent* StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDEHealthComponent* HealthComponent;


protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	UPROPERTY(EditAnywhere, Category = "Monster Base")
	class UCapsuleComponent* Capsule;
	UPROPERTY(EditAnywhere, Category = "Monster Base")
	class USkeletalMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = "Monster Base")
	class UStaticMeshComponent* TestMesh;

	UPROPERTY(EditAnywhere, Category = "EXP")
	TSubclassOf<class ADEEXPCrystal> EXPCrystal;
public:
	UPROPERTY(VisibleAnywhere)
	class UDEStatusEffectComponent* StatusEffectComponent;

protected:

	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float MoveSpeed = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float AttackDamage = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float AttackInterval = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float EXPDrop = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float DropChance = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float KnockbackResistance = 6.0f;
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	bool bIsDying = false;
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	bool bIsBoss = false;


public:
	UPROPERTY()
	FVector KnockbackVelocity;





public:
	void MoveToPlayer(float DeltaTime, const FVector& PlayerLocation);
	//void ApplyDamage(float value);
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;
	void ApplyKnockback(const FVector& Direction, float Strength = -1.0f);
	void UpdateKnockback(float DeltaTime);
	void ResetForPool();
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	float GetCollisionRadius() const;
	float GetCapsuleHalfHeight() const;
	float GetMoveSpeed() const;
	float GetDamage() const;
	float GetAttackDamage() const { return AttackDamage; }
	float GetCurrentHP() const;
	float GetMaxHP() const;
	float GetEXPDrop() const { return EXPDrop; }
	FOnMonsterDeath OnMonsterDeath;
	void DropExp();
	void ResetMonster(const struct FDEMonsterData* Data);
	bool IsBoss() { return bIsBoss; }


	bool IsAlive();
protected:
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	bool bIsAlive = false;


public:
	// ===== CC =====
	void ApplyStun(float Duration);
	bool IsStunned() const;
	void UpdateCrowdControl(float CurrentTime);

private:
	// 현재 CC 상태
	EMonsterCrowdControl CCState = EMonsterCrowdControl::None;

	// CC 종료 시간 (World Time)
	float CCEndTime = 0.f;



protected:
	void Die();


protected:
	// 겹침 시작/끝 감지 (엔진 기본 함수)
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	// 실제 데미지 주는 함수
	bool bIsTouchingPlayer = false;
	double LastAttackTime = 0.0;

public:
	// 매니저가 부를 함수
	void ExecuteAttackLogic(double CurrentTime);

	//***************** AI ****************
protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	class ADECharacterBase* TargetPlayer;
	UPROPERTY()
	class ADECharacterBase* OverlappingPlayer = nullptr;


};
