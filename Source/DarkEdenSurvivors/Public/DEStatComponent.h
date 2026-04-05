// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEStatTypes.h"
#include "DEStatComponent.generated.h"


//DECLARE_MULTICAST_DELEGATE(FOnLevelUp);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpeedChanged, float /*NewSpeed*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMagnetChanged, float /*NewRadius*/);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDEStatComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	class ADECharacterBase* OwnedChar = nullptr;


public:
	// =========================================================
	// [1] Combat Stats (���� �ɷ� - CombatComponent���� �̻��)
	// =========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Combat")
	FGameplayStat DamageMultiplier; // ���ݷ� %

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Combat")
	FGameplayStat CritChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Combat")
	FGameplayStat CritDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Combat")
	FGameplayStat CooldownReduction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Combat")
	FGameplayStat AreaSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Combat")
	FGameplayStat Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Combat")
	FGameplayStat ProjectileSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Combat")
	FGameplayStat BonusAmount; // BonusAmount

	// =========================================================
	// [2] Physical Stats (��ü �ɷ�)
	// =========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat MagnetRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat MaxHP; // ü���� ũ��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat Regeneration; // �ʴ� ȸ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat Armor; // ����

	// =========================================================
	// [3] Utility Stats (��ƿ��Ƽ/�Ĺ�)
	// =========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Utility")
	FGameplayStat Luck;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Utility")
	FGameplayStat Greed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Utility")
	FGameplayStat Growth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Utility")
	FGameplayStat Curse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Utility")
	FGameplayStat Revival; // ��Ȱ Ƚ��

	// =========================================================
	// [4] Core API (�ٽ� ���)
	// =========================================================

	// ���� ������ ���� �Լ� (� �����̵� �� ����)
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ApplyModifier(const FDEStatModifier& Mod);

	// Ǯ�� �ʱ�ȭ
	void ResetStats();

	// ���̵� ����Ʈ ó�� (�̼� ���� ��)
	void RefreshDerivedStats(EDEStatType StatType);

public:
	// �̺�Ʈ
	FOnSpeedChanged OnSpeedChanged;
	FOnMagnetChanged OnMagnetChanged;

private:
	// �� �ٽ�: Enum�� ���� �ּҸ� �����ϴ� �� (Switch�� ���ſ�)
	TMap<EDEStatType, FGameplayStat*> StatRegistry;

public:
	void InitAsPlayer(ADECharacterBase* InPlayer);

protected:
    // 영구 강화(Meta-Progression) 수치 적용
    void ApplyMetaUpgrades();

public:
		// �ܺο��� Ư�� ������ ����(����) ���� ���� �������� �Լ�
		UFUNCTION(BlueprintCallable, Category = "Stats")
		float GetStatValue(EDEStatType StatType) const;

	float GetMoveSpeed() const { return MoveSpeed.GetValue(); }
	float GetMagnetRange() const { return MagnetRange.GetValue(); }
	float GetLuck() const { return Luck.GetValue(); }
	float GetGreed() const { return Greed.GetValue(); }
	float GetCurse() const { return Curse.GetValue(); }
	float GetGrowth() const { return Growth.GetValue(); }

};
