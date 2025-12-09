// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEStatComponent.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnZeroHP);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHPChanged, float);
DECLARE_MULTICAST_DELEGATE(FOnLevelUp);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDEStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	// ====== Basic Stats ======
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MaxHP = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Stats")
	float CurrentHP = 100.f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	int Level = 1;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float CurrentEXP= 0.0f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float NextLevelEXP= 1.0f;


	// ===== Invincibility Timer =====
	UPROPERTY(EditAnywhere, Category = "Stats")
	float InvincibleTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	bool bIsPlayer = false;

	bool bInvincible = false;
	FTimerHandle InvincibleTimerHandle;
	bool CanBeDamaged() { return bInvincible; }
	void EndInvincible();
public:
	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float DamageAmount, AActor* DamageCauser = nullptr);

public:
	float GetDamage() const { return Damage; }
	float GetCurrentHP() const { return CurrentHP; }
	float GetMaxHP() const { return MaxHP; }
	float GetCurrentEXP() const { return CurrentEXP; }

public:
	void SetDamage(float v) { Damage = v; }
	void SetCurrentHP(float v) { CurrentHP = v; }
	void SetMaxHP(float v) { MaxHP = v; }
	void SetCurrentEXP(float v) { CurrentEXP = v; }
	void SetPlayer(bool IsPlayer) { bIsPlayer = IsPlayer; }

	// HP 변경 이벤트
	FOnHPChanged OnHPChanged;

	// 죽음 이벤트
	FOnZeroHP OnZeroHP;

	FOnLevelUp OnLevelUp;

	// ===== Main APIs ======
	//void ApplyDamage(float v);
	void ResetStat();
	void Heal(float Amount);
	void AddEXP(float v);
	void LevelUp();
	//void StartIFrame();

		
};
