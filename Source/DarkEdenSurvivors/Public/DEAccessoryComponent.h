// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEEffectTypes.h"
#include "DEAccessoryData.h"
#include "DEAccessoryComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnAccUpdated, const class UDEAccessoryData* /*AccessoryData*/);


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
	void LevelUpAccessory(const UDEAccessoryData* InData);

	// 악세서리 제거 (필요 시)
	UFUNCTION(BlueprintCallable, Category = "Accessory")
	void RemoveAccessory(const UDEAccessoryData* InData);

protected:
	// 정적 스탯 적용 (이동속도 등)
	void ApplyStaticStats(const UDEAccessoryData* InData);
	void RemoveStaticStats(const UDEAccessoryData* InData);

private:
private:
	// [변경] TArray -> TMap (Key: 데이터 에셋, Value: 현재 레벨)
	UPROPERTY(VisibleAnywhere, Category = "Accessory")
	TMap<const UDEAccessoryData*, int32> EquippedAccessories;

	// [캐시] 트리거별 효과 목록 (전투 중 빠른 검색용)
	// Key: OnKill, OnHit... / Value: 효과 리스트
	TMap<EEffectTrigger, TArray<FEffectData>> CachedTriggerEffects;

	// [상태] 스택형 효과의 현재 누적 스택 수 (Key: EffectType)
	TMap<EEffectType, int32> CurrentStackCounts;

	// [상태] 스택형 효과의 현재 누적 값 합계 (Key: EffectType)
	TMap<EEffectType, float> CurrentStackValues;

public:
	// UI가 가져다 쓸 수 있도록 TMap 반환
	const TMap<const UDEAccessoryData*, int32>& GetEquippedAccessories() const { return EquippedAccessories; }

	FOnAccUpdated OnAccUpdated;

protected:
	class UDEInventoryComponent* CachedInventoryComp;

public:
	void SetInventoryComp(class UDEInventoryComponent* InInventory) { CachedInventoryComp = InInventory; }

};
