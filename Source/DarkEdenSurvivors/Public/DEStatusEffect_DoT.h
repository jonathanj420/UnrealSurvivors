// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEStatusEffectBase.h"
#include "DEDamageTypes.h"
#include "DEStatusEffect_DoT.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEStatusEffect_DoT : public UDEStatusEffectBase
{
	GENERATED_BODY()

public:
	UDEStatusEffect_DoT();

	// --- 기획 데이터 (블루프린트에서 설정) ---

	//// 데미지 속성 (물리 베기, 산성 독, 마법 등)을 구분하기 위한 클래스
public:
	// =========================================================
	// ★ [NEW] 틱뎀이 들어갈 때 데미지 청구서에 찍힐 속성 태그들!
	// 기획자가 블루프린트에서 "Damage.Affinity.Poison" 등을 세팅합니다.
	// =========================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect|Damage", meta = (Categories = "Damage"))
	FGameplayTagContainer DoTDamageTags;
	// 틱(Interval)마다 타겟 몸에서 터질 이펙트 (예: 피가 튀거나 독 방울이 터지는 효과)
	// 다크에덴 특유의 세피아 톤이나 칙칙한 녹색 이펙트를 여기에 할당합니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Effect|Visual")
	class UNiagaraSystem* TickParticle;

	// --- 핵심 로직 오버라이드 ---
	virtual void OnIntervalTick(AActor* Target, FActiveStatusEffect& EffectData) const override;
	
};
