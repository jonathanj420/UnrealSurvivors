// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAccessoryComponent.h"
#include "DEStatComponent.h"
#include "GameFramework/Actor.h"

UDEAccessoryComponent::UDEAccessoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // 틱 필요 없음 (성능 최적화)
}

void UDEAccessoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

// =========================================================
// 1. 장착 관리
// =========================================================

void UDEAccessoryComponent::AddAccessory(const UDEAccessoryData* InData)
{
	if (!InData) return;

	// 목록에 추가
	EquippedAccessories.Add(InData);

	// 1. 정적 스탯(이동속도 등) 즉시 적용
	ApplyStaticStats(InData);

	// 2. 동적 효과(OnKill 등) 캐싱
	RebuildEffectCache();
}

void UDEAccessoryComponent::RemoveAccessory(const UDEAccessoryData* InData)
{
	if (!InData) return;

	if (EquippedAccessories.Remove(InData) > 0) // 하나라도 지워졌다면
	{
		// 정적 스탯 제거
		RemoveStaticStats(InData);

		// 캐시 다시 굽기
		RebuildEffectCache();
	}
}

void UDEAccessoryComponent::RebuildEffectCache()
{
	// 기존 캐시 초기화
	CachedTriggerEffects.Empty();

	// 모든 악세서리를 순회하며 트리거별로 분류 (Bucketing)
	for (const UDEAccessoryData* Data : EquippedAccessories)
	{
		if (!Data) continue;

		for (const FEffectData& Effect : Data->Effects)
		{
			// None 트리거는 캐싱 안 함 (상시 적용 패시브 등은 별도 처리하거나 여기 포함 안 됨)
			if (Effect.Trigger == EEffectTrigger::None) continue;

			// 맵에 배열이 없으면 만들고 추가 (FindOrAdd)
			CachedTriggerEffects.FindOrAdd(Effect.Trigger).Add(Effect);
		}
	}
}

void UDEAccessoryComponent::ApplyStaticStats(const UDEAccessoryData* InData)
{
	// 캐릭터의 StatComponent 찾기
	if (UDEStatComponent* StatComp = GetOwner()->FindComponentByClass<UDEStatComponent>())
	{
		for (const FDEStatModifier& Mod : InData->StatModifiers)
		{
			StatComp->ApplyModifier(Mod);
		}
	}
}

void UDEAccessoryComponent::RemoveStaticStats(const UDEAccessoryData* InData)
{
	// 제거 로직은 StatComponent에 RemoveModifier가 있거나, 
	// 혹은 StatComponent를 Reset하고 전체 재계산하는 방식 추천
	// 여기서는 개념적으로만 작성:
	/*
	if (UDEStatComponent* StatComp = GetOwner()->FindComponentByClass<UDEStatComponent>())
	{
		for (const FDEStatModifier& Mod : InData->StatModifiers)
		{
			StatComp->RemoveModifier(Mod);
		}
	}
	*/
}

// =========================================================
// 2. 쿼리 (CombatComponent가 사용)
// =========================================================

const TArray<FEffectData>& UDEAccessoryComponent::GetEffectsByTrigger(EEffectTrigger InTrigger) const
{
	// 미리 분류해둔 배열이 있으면 반환 (O(1))
	if (const TArray<FEffectData>* FoundEffects = CachedTriggerEffects.Find(InTrigger))
	{
		return *FoundEffects;
	}

	// 없으면 빈 배열 반환 (static으로 선언해 메모리 낭비 방지)
	static const TArray<FEffectData> EmptyArray;
	return EmptyArray;
}

float UDEAccessoryComponent::GetTotalStackValue(EEffectType InType) const
{
	// 해당 타입의 누적된 값 반환 (예: 데미지 증가량 총합)
	if (const float* Val = CurrentStackValues.Find(InType))
	{
		return *Val;
	}
	return 0.0f;
}

// =========================================================
// 3. 상태 변경 (CombatComponent가 실행)
// =========================================================

void UDEAccessoryComponent::AddStack(EEffectType InType, float InValue, int32 InMaxStack)
{
	// 1. 현재 스택 수 가져오기
	int32& CurrentCount = CurrentStackCounts.FindOrAdd(InType);

	// 2. 최대 스택 체크
	if (CurrentCount < InMaxStack)
	{
		// 3. 스택 증가
		CurrentCount++;

		// 4. 값 누적 (예: 0.1씩 증가)
		float& CurrentValue = CurrentStackValues.FindOrAdd(InType);
		CurrentValue += InValue;

		// (로그 디버깅용)
		// UE_LOG(LogTemp, Log, TEXT("Stack Added! Type: %d, Count: %d, TotalVal: %f"), (int32)InType, CurrentCount, CurrentValue);
	}
}