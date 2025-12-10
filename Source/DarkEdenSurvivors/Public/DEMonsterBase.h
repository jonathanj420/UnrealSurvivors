// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/Actor.h"
#include "DEMonsterBase.generated.h"

//class ADEMonsterBase;
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDeath, ADEMonsterBase*, Monster);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterDeath, ADEMonsterBase*, Monster);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMonsterDeath, ADEMonsterBase*);

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
protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	UPROPERTY(EditAnywhere, Category = "Monster Base")
	class UCapsuleComponent* Capsule;
	UPROPERTY(EditAnywhere, Category = "Monster Base")
	class USkeletalMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = "Monster Base")
	class UStaticMeshComponent* TestMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	class UDEStatComponent* StatComp;
	UPROPERTY(EditAnywhere, Category = "EXP")
	TSubclassOf<class ADEEXPCrystal> EXPCrystal;
	UPROPERTY()
	class ADEMonsterSpawnManager* SpawnManager;

protected:

	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float MoveSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float EXPDrop = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	bool bIsDying = false;
	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	bool bIsBoss = false;


public:
	UPROPERTY()
	FVector KnockbackVelocity;

	UPROPERTY(EditAnywhere, Category = "Monster Stats")
	float KnockbackResistance = 6.0f;


public:
	void MoveToPlayer(float DeltaTime);
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
	float GetDamage();
	float GetCurrentHP();
	FOnMonsterDeath OnMonsterDeath;
	void DropExp();
	void ResetMonster();
	bool IsBoss() { return bIsBoss; }
	bool IsActive();

protected:
	void Die();


	//***************** AI ****************
protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	class ADEFemaleVampire* TargetPlayer;


};
