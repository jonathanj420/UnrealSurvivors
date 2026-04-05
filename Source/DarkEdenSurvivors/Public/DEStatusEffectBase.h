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

	// (선택) 시각 효과: 다크에덴 특유의 세피아 톤, 테두리 없는(Borderless) 고딕 이펙트 등
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect|Visual")
	class UNiagaraSystem* EffectParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect|Visual")
	bool bApplyMaterialTint = false;

	// 머티리얼에서 색상을 조작할 파라미터 이름 (예: "BaseColor", "Tint" 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect|Visual", meta = (EditCondition = "bApplyMaterialTint"))
	FName TintParameterName = TEXT("ColorTint");

	// 상태이상이 걸렸을 때 칠할 색상 (맹독 = 초록색)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect|Visual", meta = (EditCondition = "bApplyMaterialTint"))
	FLinearColor TintColor = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); // 기본값 초록색

	// 상태이상이 끝났을 때 돌아갈 원래 색상 (보통 흰색)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect|Visual", meta = (EditCondition = "bApplyMaterialTint"))
	FLinearColor OriginalColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// --- 핵심 로직 (모든 함수는 const로 선언되어 객체 자체의 변형을 막습니다) ---

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
