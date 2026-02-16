// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/Character.h"
#include "DESkillBase.h" // UDESkillBase 클래스 정의 포함
#include "DECharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerDiedDelegate);

class UDECombatComponent;
class UDEStatComponent;
class UDEHealthComponent;
class UDEProgressionComponent;

UCLASS()
class DARKEDENSURVIVORS_API ADECharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADECharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// 스킬 매니저 (모든 캐릭터가 가짐)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDESkillManagerComponent* SkillManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDEInventoryComponent* Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDEHealthComponent* HealthComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDEStatComponent* StatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDECombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDEProgressionComponent* ProgressionComponent;

	//***CAMERA***
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY()
	class ADEPlayerController* DEPlayerController;

	/*UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* WeaponL;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* WeaponR;


	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AABWeapon* CurrentWeapon;*/
	enum class EControlMode
	{
		FIXEDTPS,
		FREETPS,
		FPS,
	};
	void SetControlMode(EControlMode NewControlMode);
	EControlMode CurrentControlMode = EControlMode::FREETPS;
	FVector DirectionToMove = FVector::ZeroVector;

	float ArmLengthTo = 0.0f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.0f;
	float ArmRotationSpeed = 0.0f;
	bool IsFPS;
	bool bMoveCamera;
	float IFrame = 0.5;
protected:


private:
	UFUNCTION()
	void MoveForward(float value);

	UFUNCTION()
	void MoveRight(float value);

	UFUNCTION()
	void LookUp(float value);

	UFUNCTION()
	void Turn(float value);

	UFUNCTION()
	void ViewChange();

	//***ATTACK***
	UFUNCTION()
	void BloodDrain();
	UPROPERTY(VisibleinstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool bCanMove;
public:
	void SetCanMove(bool bInCanMove);
	bool CanMove() const { return bCanMove; }
protected:
	bool bAutoSkillsPaused = false;

public:
	void SetAutoSkillsPaused(bool bPaused);
	bool IsAutoSkillsPaused() const { return bAutoSkillsPaused; }

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void Heal(float Amount);
	void AddExp(float v);
public:
	// 캐릭터가 기본적으로 가지고 태어나는 스킬 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	int32 BaseSkillID = -1;

	UFUNCTION()
	void OnCharacterLevelUp(int32 NewLevel);

	UFUNCTION()
	UDESkillManagerComponent* GetSkillManagerComponent() { return SkillManager; }

	TMap<FName, TWeakObjectPtr<class ADESimpleAOEBase>> ActiveAOEMap;

protected:
	UPROPERTY()
	class UDEActiveSkillBase* ActiveSkill;

	//UPROPERTY()
	void OnActiveSkillInput();

	UPROPERTY()
	TArray<class UDEActiveSkillBase*> ActiveSkills;

	int32 CurrentActiveSkillIndex = 0;
	void UseActiveSkill();

	float BloodDrainGauge;
	float BloodDrainGaugeMax;
	float BloodDrainGainPerKill;

public:
	void AddBloodDrainGauge(float Amount);
	void ConsumeBloodDrainGauge();
	bool CanActivateBloodDrain();
	float GetBloodDrainGainPerKill();
	bool IsDead() const;
	void ForceKill() { Die(); }
protected:
	UFUNCTION()
	virtual void Die();

public:
	float GetCapsuleHalfRadius();
public:
	// [4] 접근자 (Getter) - 다른 클래스에서 편하게 가져다 쓰라고 만듦
	UFUNCTION(BlueprintCallable, Category = "Components")
	UDEStatComponent* GetStatComponent() const { return StatComponent; }

	UFUNCTION(BlueprintCallable, Category = "Components")
	UDECombatComponent* GetCombatComponent() const { return CombatComponent; }

	UFUNCTION(BlueprintCallable, Category = "Components")
	UDEHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UFUNCTION(BlueprintCallable, Category = "Components")
	UDEProgressionComponent* GetProgressionComponent() const { return ProgressionComponent; }


public:

	FOnPlayerDiedDelegate OnPlayerDied;

public:
	void MyDebugCheat();
	UFUNCTION(Exec)
	void ForceLevelUp();
};