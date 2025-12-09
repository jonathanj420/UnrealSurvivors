// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "TestProjectile.h"
#include "TestCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class DARKEDENSURVIVORS_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestCharacter();
	void SetCharacterState(ECharacterState NewState);
	ECharacterState GetCharacterState() const;
	int32 GetExp() const;
	float GetFinalAttackRange() const;
	float GetFinalAttackDamage() const;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 스폰할 발사체 클래스입니다.
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<class ADEBloodyKnife> ProjectileClass;

	enum class EControlMode
	{
		FIXEDTPS,
		FREETPS,
		FPS,
		NPC
	};
	void SetControlMode(EControlMode NewControlMode);
	EControlMode CurrentControlMode = EControlMode::FREETPS;
	FVector DirectionToMove = FVector::ZeroVector;

	float ArmLengthTo = 0.0f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.0f;
	float ArmRotationSpeed = 0.0f;
	bool IsFPS;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void PossessedBy(AController* NewController) override;


	

	UPROPERTY(VisibleAnywhere,Category=Weapon)
	USkeletalMeshComponent* WeaponL;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* WeaponR;


	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AABWeapon* CurrentWeapon;

	UPROPERTY(EditAnywhere, Category = Stat)
	class UABCharacterStatComponent* CharacterStat;

	// FPS 카메라
	//UPROPERTY(VisibleAnywhere)
	//UCameraComponent* FPSCameraComponent;

	//탑다운 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;

	UFUNCTION()
	void BasicAttack();
	FOnAttackEndDelegate OnAttackEnd;

	//Blood Drain
	//void BloodDrain();



	bool CanSetWeapon();
	void SetWeapon(class AABWeapon* NewWeapon);
	void SetDualWeapon(class AABWeapon* NewWeaponR, class AABWeapon* NewWeaponL);
	


	//DUAL WIELDING
	/*UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AABWeapon* CurrentWeaponR;
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AABWeapon* CurrentWeaponL;*/

	//// 일인칭 메시(팔)로, 소유 플레이어에게만 보입니다.
	//UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	//USkeletalMeshComponent* FPSMesh;


	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;*/

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
	void StartJump();

	UFUNCTION()
	void StopJump();

	void ViewChange();

	
	private:

	void OnAssetLoadCompleted();

	int32 AssetIndex = 0;

	private:
		FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
		TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;

		UPROPERTY(Transient,VisibleInstanceOnly,BlueprintReadOnly,Category=State,Meta=(AllowPrivateAccess=true))
		ECharacterState CurrentState;

		UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
		bool bIsPlayer;

		UPROPERTY()
		class AABAIController* ABAIController;

		UPROPERTY()
		class AABPlayerController* ABPlayerController;

	
	
	UFUNCTION()
	void BulletAttack();

	UFUNCTION()
	void BloodDrain();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackStartComboState();
	void AttackEndComboState();
	void AttackCheck();



private:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category=Attack,Meta=(AllowPrivateAccess=true))
	bool CanNextCombo;

	UPROPERTY(VisibleinstanceOnly,BlueprintReadOnly,Category=Attack,Meta=(AllowPrivateAccess=true))
	bool IsComboInputOn;

	UPROPERTY(VisibleinstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleinstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	UPROPERTY(VisibleinstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool bIsMovable;

	UPROPERTY()
	class UABAnimInstance* ABAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category=State,Meta=(AllowPrivateAccess=true))
	float DeadTimer;

	FTimerHandle DeadTimerHandle = {};

	FTimerHandle AutoAttackTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackInterval = 1.0f; // 1초 간격

	void HandleAutoAttack();  // 공격 실행 함수
	void StartAutoAttack();   // 타이머 시작
	void StopAutoAttack();    // 타이머 중지

};
