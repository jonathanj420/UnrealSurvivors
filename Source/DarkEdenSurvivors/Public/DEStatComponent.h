// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEStatTypes.h"
#include "DEStatComponent.generated.h"


//DECLARE_MULTICAST_DELEGATE(FOnLevelUp);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpeedChanged, float /*NewSpeed*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMagnetChanged, float /*NewRadius*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, EDEStatType /*StatType*/, float /*NewValue*/);

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

	// 피격 후 무적시간(초). 접촉 피해가 몬스터 수에 비례해 쌓이지 않게 한다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat InvulnerabilityTime;

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
	// 모든 캐릭터 (플레이어, AI 마스터): 소유 캐릭터를 잡고 파생 값(이동속도, 최대 HP 등)을 반영한다
	void InitForCharacter(ADECharacterBase* InCharacter);

    // 영구 강화(Meta-Progression) 수치 적용. 플레이어가 조종하는 캐릭터만, 한 번만 부른다 (ADECharacterBase::TryApplyPlayerSetup)
    void ApplyMetaUpgrades();

public:
	FOnStatChanged OnStatChanged;
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
