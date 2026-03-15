// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEStatTypes.h"
#include "DEDamageTypes.h"
#include "DECombatEffect.h"
#include "DECombatComponent.generated.h"

class UDEStatComponent;
class UDEAccessoryComponent;


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
	// [2] 핵심 기능 (API)
	// =========================================================

	/**
	 * 현재 플레이어의 전투 능력을 캡처해서 반환합니다.
	 * 스킬 발동 시(BuildContext) 딱 한 번 호출됩니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FCombatSnapshot GetCombatSnapshot() const;


protected:
	//// 내부 로직 분리 (protected나 private 권장)

	//// 생명력 흡수 처리
	//void ProcessLifeSteal(const FCombatSnapshot& Snapshot);

	//// 처치 시 효과 처리 (킬 카운트, 쿨감, 폭발 등)
	//void ProcessOnKillEffect(AActor* Victim);

	// 컴포넌트 캐싱 (매번 FindComponent 안 하려고)
	UPROPERTY()
	UDEStatComponent* CachedStatComp;

	UPROPERTY()
	UDEAccessoryComponent* CachedAccessoryComp;

protected:
	int32 TotalKillCount = 0;
	float TotalDamageDealt = 0.f;

protected:
	// ★ 통합 레고 바구니 (Instanced 필수)
	UPROPERTY(EditAnywhere, Instanced, Category = "Combat Effects")
	TArray<UDECombatEffect*> ActiveCombatEffects;

public:
	// ★ 효과 추가/제거용 함수 (C++ 런타임 적용용)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AddCombatEffect(UDECombatEffect* NewEffect);

	// ★ 동네방네 사건을 알리는 핵심 방송국 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void BroadcastCombatEvent(ECombatEventTrigger TriggerType, FCombatEventData& EventData);

	// 데미지를 줬을 때 처리 (내용 수정할 거임)
	void HandleDamageDealt(const FDEDamageResult& Result, const FCombatSnapshot& Snapshot);

};
