// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEEffectTypes.h"
#include "DEAccessoryData.h"
#include "DEAccessoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEAccessoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDEAccessoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	// =========================================================
	// 1. 장착 관리
	// =========================================================

	// 악세서리 추가 (인벤토리에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Accessory")
	void AddAccessory(const UDEAccessoryData* InData);

	// 악세서리 제거 (필요 시)
	UFUNCTION(BlueprintCallable, Category = "Accessory")
	void RemoveAccessory(const UDEAccessoryData* InData);

	// =========================================================
	// 2. 쿼리 (CombatComponent가 사용)
	// =========================================================

	// 특정 트리거(OnKill 등)에 해당하는 효과 리스트 반환 (O(1) 조회)
	const TArray<FEffectData>& GetEffectsByTrigger(EEffectTrigger InTrigger) const;

	// 특정 효과 타입(StackDamage 등)의 현재 누적 값 반환 (Snapshot용)
	float GetTotalStackValue(EEffectType InType) const;

	// =========================================================
	// 3. 상태 변경 (CombatComponent가 실행)
	// =========================================================

	// 스택 쌓기 요청
	void AddStack(EEffectType InType, float InValue, int32 InMaxStack);

protected:
	// 내부 캐시 재구축 (장착/해제 시 호출)
	void RebuildEffectCache();

	// 정적 스탯 적용 (이동속도 등)
	void ApplyStaticStats(const UDEAccessoryData* InData);
	void RemoveStaticStats(const UDEAccessoryData* InData);

private:
	// 장착된 악세서리 목록
	UPROPERTY(VisibleAnywhere, Category = "Accessory")
	TArray<const UDEAccessoryData*> EquippedAccessories;

	// [캐시] 트리거별 효과 목록 (전투 중 빠른 검색용)
	// Key: OnKill, OnHit... / Value: 효과 리스트
	TMap<EEffectTrigger, TArray<FEffectData>> CachedTriggerEffects;

	// [상태] 스택형 효과의 현재 누적 스택 수 (Key: EffectType)
	TMap<EEffectType, int32> CurrentStackCounts;

	// [상태] 스택형 효과의 현재 누적 값 합계 (Key: EffectType)
	TMap<EEffectType, float> CurrentStackValues;

		
};
