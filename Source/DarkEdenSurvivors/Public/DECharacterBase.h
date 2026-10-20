// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/Character.h"
#include "DECharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerDiedDelegate);

class UDECombatComponent;
class UDEStatComponent;
class UDEHealthComponent;
class UDEProgressionComponent;
class UDEAccessoryComponent;
class UDELevelUpManagerComponent;

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

	// 플레이어 컨트롤러에 빙의됐을 때 플레이어 전용 설정을 적용한다
	virtual void PossessedBy(AController* NewController) override;

	// 플레이어가 조종하는 캐릭터에만 하는 설정 (메타 업그레이드, 픽업용 "Player" 태그, 피격 숫자 숨김).
	// AI 마스터는 받지 않는다. BeginPlay와 빙의 중 늦게 오는 쪽에서 한 번만 적용된다
	void TryApplyPlayerSetup();
	bool bPlayerSetupApplied = false;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDEAccessoryComponent* AccessoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDELevelUpManagerComponent* LevelUpManagerComponent;

	//***CAMERA***
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* Camera;

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

	UPROPERTY(VisibleinstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool bCanMove;
public:
	void SetCanMove(bool bInCanMove);
	bool CanMove() const { return bCanMove; }

	// 자동 스킬 발동 멈춤 (흡혈 시전 중 등). 상태는 SkillManager가 갖는다
	void SetAutoSkillsPaused(bool bPaused);

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void Heal(float Amount);
	void AddExp(float v);
	void SetMaxHP(float NewMaxHP);
public:
	// 캐릭터가 기본적으로 가지고 태어나는 스킬 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	int32 BaseSkillID = -1;

	UFUNCTION()
	void OnCharacterLevelUp(int32 NewLevel);

protected:
	// CharacterData의 마일스톤 중 RequiredLevel이 (FromLevel, ToLevel]인 것을 스탯에 적용한다
	void ApplyLevelMilestones(int32 FromLevel, int32 ToLevel);

public:

	UFUNCTION()
	UDESkillManagerComponent* GetSkillManagerComponent() { return SkillManager; }

protected:
	UPROPERTY()
	class UDEActiveSkillBase* ActiveSkill;

	void OnActiveSkillInput();

	float BloodDrainGauge;
	float BloodDrainGaugeMax;
	float BloodDrainGainPerKill;

public:
	void AddBloodDrainGauge(float Amount);
	void ConsumeBloodDrainGauge();
	bool CanActivateBloodDrain();
	float GetBloodDrainGainPerKill();
	bool IsDead() const;
protected:
	UFUNCTION()
	virtual void Die();

	// 피격 플래시: 맞으면 메시 머티리얼의 이 스칼라 파라미터를 1로 올렸다가 HitFlashDuration 뒤 0으로 되돌린다.
	// 머티리얼에 파라미터가 없으면 아무 일도 없다
	UPROPERTY(EditDefaultsOnly, Category = "Feedback")
	FName HitFlashParameterName = TEXT("HitFlash");

	UPROPERTY(EditDefaultsOnly, Category = "Feedback", meta = (Units = "s"))
	float HitFlashDuration = 0.12f;

	FTimerHandle HitFlashTimerHandle;

	void OnDamageTaken(float Damage, AActor* DamageInstigator);
	void EndHitFlash();

public:
	float GetCapsuleRadius() const;
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

	UFUNCTION(BlueprintCallable, Category = "Components")
	UDELevelUpManagerComponent* GetLevelUpManagerComponent() const { return LevelUpManagerComponent; }


public:

	FOnPlayerDiedDelegate OnPlayerDied;

public:
	// 디버그 치트. Shipping 빌드에서는 입력 바인딩이 없고 본문이 비어 있다
	// (UFUNCTION은 #if로 감쌀 수 없어서 선언은 남긴다)
	void MyDebugCheat();
	void MyAnotherDebugCheat();
	UFUNCTION(Exec)
	void ForceLevelUp();
	UFUNCTION(Exec)
	void ForceLevelUpSkill(int32 SkillId);

public:
	// 캐릭터별 데이터 (레벨 마일스톤 패시브). 캐릭터 BP에서 지정한다
	UPROPERTY(EditDefaultsOnly, Category = "Character Data")
	class UDECharacterData* CharacterData;
};