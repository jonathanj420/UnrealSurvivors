// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/Actor.h"
#include "Data/DEMonsterData.h"
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
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDEStatComponent* StatComponent;*/

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
	float KnockbackResistance = 6.0f;
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	bool bIsDying = false;

public:
	// 내 등급이 뭔지 저장하는 변수 (블루프린트에서 기획자가 몹마다 딸깍 설정 가능)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Data")
	EDEMonsterRank MonsterRank = EDEMonsterRank::Normal;

	// 등급을 반환하는 Get 함수
	EDEMonsterRank GetMonsterRank() const { return MonsterRank; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster|Drop")
	TArray<FDEMonsterDropInfo> DropTable;

public:
	UPROPERTY()
	FVector KnockbackVelocity;
	FVector PendingOverlapPush = FVector::ZeroVector;
	FRotator PendingRotation = FRotator::ZeroRotator;
public:
	// 매 프레임 위치와 반지름을 딱 한 번만 캐싱해둘 변수
	FVector CachedLocation = FVector::ZeroVector;
	float CachedRadius = 42.0f;

public:
	void MoveToPlayer(float DeltaTime, const FVector& PlayerLocation, const FVector& MyLocation);
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
	FOnMonsterDeath OnMonsterDeath;
	void ResetMonster(const struct FDEMonsterData* Data);
	void SetCanMove(bool bNewCanMove);

	// 매니저가 고속으로 읽어갈 상태 플래그
	bool bCanMove = true;

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
	void StartDying();
	public:

	void ExecuteFinalDeath();


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
