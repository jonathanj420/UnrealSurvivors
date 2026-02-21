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
	// [1] Combat Stats (전투 능력 - CombatComponent에서 이사옴)
	// =========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Combat")
	FGameplayStat DamageMultiplier; // 공격력 %

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
	// [2] Physical Stats (신체 능력)
	// =========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat MagnetRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat MaxHP; // 체력통 크기

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat Regeneration; // 초당 회복량

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat Armor; // 방어력

	// =========================================================
	// [3] Utility Stats (유틸리티/파밍)
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
	FGameplayStat Revival; // 부활 횟수

	// =========================================================
	// [4] Core API (핵심 기능)
	// =========================================================

	// 통합 수정자 적용 함수 (어떤 스탯이든 다 받음)
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void ApplyModifier(const FDEStatModifier& Mod);

	// 풀링 초기화
	void ResetStats();

	// 사이드 이펙트 처리 (이속 변경 등)
	void RefreshDerivedStats(EDEStatType StatType);

public:
	// 이벤트
	FOnSpeedChanged OnSpeedChanged;
	FOnMagnetChanged OnMagnetChanged;

private:
	// ★ 핵심: Enum과 변수 주소를 연결하는 맵 (Switch문 제거용)
	TMap<EDEStatType, FGameplayStat*> StatRegistry;

public:
	void InitAsPlayer(ADECharacterBase* InPlayer);

public:
	float GetMoveSpeed() const { return MoveSpeed.GetValue(); }
	float GetMagnetRange() const { return MagnetRange.GetValue(); }
	float GetLuck() const { return Luck.GetValue(); }
	float GetGreed() const { return Greed.GetValue(); }
	float GetCurse() const { return Curse.GetValue(); }
	float GetGrowth() const { return Growth.GetValue(); }

};
