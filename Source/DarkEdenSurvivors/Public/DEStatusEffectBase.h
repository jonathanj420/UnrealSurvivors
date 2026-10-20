// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Data/DEStatusEffectTypes.h"
#include "DEDamageTypes.h"
#include "DEStatusEffectBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class DARKEDENSURVIVORS_API UDEStatusEffectBase : public UObject
{
	GENERATED_BODY()

public:
	UDEStatusEffectBase();

	// --- 기획자가 에디터에서 세팅할 변하지 않는 데이터 (CDO) ---
	// ★ [NEW] 이 상태이상의 정체성 (예: Status.Debuff.Poison, Status.Buff.MoveSpeed)
	// 나중에 해제 스킬(정화)을 만들 때 이 명찰을 보고 지웁니다.
	// =========================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect|Tags", meta = (Categories = "Status"))
	FGameplayTagContainer StatusTags;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect|Core")
	EStackPolicy StackPolicy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect|Core")
	int32 MaxStacks;

	// Stack 정책에서 재적용 시 지속시간을 갱신할지. 갱신은 남은 시간과 새 지속시간 중 긴 쪽 (Refresh와 같은 규칙)
	UPROPERTY(EditDefaultsOnly, Category = "Status Effect|Core")
	bool bRefreshDurationOnStack = true;

	// --- 핵심 로직 (모든 함수는 const로 선언되어 객체 자체의 변형을 막습니다) ---

	// 블루프린트용 UFUNCTION, UPARAM(ref), _Implementation 꼬리표 모두 삭제!
	virtual void OnApply(AActor* Target, FActiveStatusEffect& EffectData) const;

	virtual void OnRemove(AActor* Target, FActiveStatusEffect& EffectData) const;

	virtual void OnIntervalTick(AActor* Target, FActiveStatusEffect& EffectData) const;

	virtual void OnStacked(AActor* Target, FActiveStatusEffect& EffectData, int32 NewStackCount) const;

	// Tick은 매 프레임 수백~수천 번 불리므로 원래부터 순수 가상 함수였습니다. 유지!
	virtual void Tick(AActor* Target, FActiveStatusEffect& EffectData, float DeltaTime) const;

	virtual void ModifyIncomingDamage(const FActiveStatusEffect& EffectData, FDEDamageRequest& InOutRequest) const {};
};
