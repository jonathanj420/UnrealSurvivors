// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Data/DEStatusEffectTypes.h"
#include "DEStatusEffectBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class DARKEDENSURVIVORS_API UDEStatusEffectBase : public UObject
{
	GENERATED_BODY()

public:
	// 초기화 (누가, 누구에게, 얼마나, 강도는?) - ADEMonsterBase에서 AActor로 변경!
	virtual void InitEffect(AActor* InInstigator, AActor* InTarget, float InDuration, float InPower, float InInterval = 0.f);

	// 컴포넌트가 매 프레임 호출해 줄 Tick 함수
	virtual void Tick(float DeltaTime);

	// 종료 여부 체크
	bool IsFinished() const { return Duration > 0.f && ElapsedTime >= Duration; }

	// ───  이벤트 함수들 (C++ 구현 + BP에서 이펙트 추가 가능) ───

	// 시작될 때 (예: 스턴 걸림 -> 움직임 멈춤)
	UFUNCTION(BlueprintNativeEvent, Category = "StatusEffect")
	void OnApply();

	// 끝날 때 (예: 스턴 풀림 -> 움직임 재개)
	UFUNCTION(BlueprintNativeEvent, Category = "StatusEffect")
	void OnRemove();

	// 틱마다 (예: 독 데미지 들어가는 순간)
	UFUNCTION(BlueprintNativeEvent, Category = "StatusEffect")
	void OnIntervalTick();

	// 스택이 쌓였을 때 (예: 출혈 스택 증가 시 이펙트 폭발)
	UFUNCTION(BlueprintNativeEvent, Category = "StatusEffect")
	void OnStacked(int32 NewStackCount);

public:
	// ───  에디터(블루프린트) 세팅용 데이터 (데이터 주도 설계!) ───
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StatusEffect|Policy")
	EEffectTag EffectTag = EEffectTag::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StatusEffect|Policy")
	EStackPolicy StackPolicy = EStackPolicy::Replace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StatusEffect|Policy", meta = (EditCondition = "StackPolicy == EStackPolicy::Stack"))
	int32 MaxStacks = 1;

	// ─── 런타임 데이터 ───
	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect|State")
	int32 CurrentStacks = 1;

	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect|State")
	AActor* Instigator; // 시전한 사람

	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect|State")
	AActor* Target; // 당한 대상 (플레이어 or 몬스터)

	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect|State")
	float Duration;     // 지속 시간

	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect|State")
	float ElapsedTime;  // 경과 시간

	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect|State")
	float Power;        // 위력 (데미지 양 or 슬로우 비율)

	UPROPERTY(BlueprintReadOnly, Category = "StatusEffect|State")
	float Interval;     // 틱 간격 (도트딜용)

	float TickTimer;    // 내부 틱 계산용
};
