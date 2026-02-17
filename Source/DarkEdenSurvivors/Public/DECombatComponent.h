// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEStatTypes.h"
#include "DEDamageTypes.h"
#include "DECombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDECombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDECombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// =========================================================
	// [1] 전투 스탯 (플레이어의 능력치)
	// =========================================================

	// 공격력 (퍼센트 증가용. 깡공은 SkillData에 있음)
	// 예: Base=1.0, Item=+0.5 -> 1.5배 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	FGameplayStat DamageMultiplier;

	// 치명타 확률 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	FGameplayStat CritChance;

	// 치명타 피해 배율 (기본 1.5 = 150%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	FGameplayStat CritDamageMultiplier;

	// 쿨타임 감소 (최대 0.8 등으로 제한 필요)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	FGameplayStat CooldownReduction;

	// 범위 크기 (Area)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	FGameplayStat AreaSize;

	// 지속 시간 (Duration)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	FGameplayStat Duration;

	// 투사체 속도 (Speed)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	FGameplayStat Speed;

	// 추가 투사체 개수 (Amount) - 이건 보통 정수지만 계산을 위해 float로 관리 후 내림
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	FGameplayStat BonusAmount;


	// =========================================================
	// [2] 핵심 기능 (API)
	// =========================================================

	/**
	 * 현재 플레이어의 전투 능력을 캡처해서 반환합니다.
	 * 스킬 발동 시(BuildContext) 딱 한 번 호출됩니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FCombatSnapshot GetCombatSnapshot() const;

		
public:
	/**
	 * 전투 결과 처리 (Post-Process)
	 * 스킬이나 투사체가 데미지를 입힌 직후에 호출합니다.
	 * @param Result : 피해자(HealthComponent)가 리턴한 데미지 결과
	 * @param Snapshot : 공격 당시의 내 스탯 (피흡 확률 등 포함)
	 */
	void HandleDamageDealt(const FDEDamageResult& Result, const FCombatSnapshot& Snapshot);

protected:
	// 내부 로직 분리 (protected나 private 권장)

	// 생명력 흡수 처리
	void ProcessLifeSteal(const FCombatSnapshot& Snapshot);

	// 처치 시 효과 처리 (킬 카운트, 쿨감, 폭발 등)
	void ProcessOnKillEffect(AActor* Victim);

};
