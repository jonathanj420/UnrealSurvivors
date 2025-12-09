// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/Character.h"
#include "DESkillBase.h" // UDESkillBase 클래스 정의 포함
#include "DECharacterBase.generated.h"

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
	class UDEStatComponent* StatComponent; // 포워드 선언된 UStatsComponent 포인터
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
	bool bIsMovable;

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void AddEXP(float v);
public:
	// 캐릭터가 기본적으로 가지고 태어나는 스킬 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	int32 BaseSkillID = -1;

	UFUNCTION()
	void OnCharacterLevelUp();

	UFUNCTION()
	UDESkillManagerComponent* GetSkillManagerComponent() { return SkillManager; }
	

};