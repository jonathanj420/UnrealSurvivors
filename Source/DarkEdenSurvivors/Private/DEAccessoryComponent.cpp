// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAccessoryComponent.h"
#include "DEStatComponent.h"
#include "DEInventoryComponent.h"
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

void UDEAccessoryComponent::LevelUpAccessory(const UDEAccessoryData* InData)
{
	if (!InData) return;

	// 맵에 있으면 현재 레벨을 가져오고, 없으면 0으로 시작해서 레퍼런스(&)로 가져옵니다.
	int32& CurrentLevel = EquippedAccessories.FindOrAdd(InData, 0);

	// 최대 레벨 검사
	if (CurrentLevel < InData->MaxLevel)
	{
		// 1. 레벨 1 증가
		CurrentLevel++;

		// 2. 스탯 적용 (우리가 짠 StatComponent가 알아서 중첩 연산 해줌!)
		ApplyStaticStats(InData);

		// 3. 효과 캐시도 다시 구워줌 (Effects가 있다면)
		RebuildEffectCache();

		UE_LOG(LogTemp, Warning, TEXT("Accessory %s -> Lv.%d"), *InData->Name.ToString(), CurrentLevel);

		CachedInventoryComp->TryAddAccessory(InData->ID);

		// 4. UI 갱신 알람 발송!
		OnAccUpdated.Broadcast(InData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Accessory] %s is already Max Level (%d)!"), *InData->Name.ToString(), InData->MaxLevel);
	}

}

void UDEAccessoryComponent::AddAccessory(const UDEAccessoryData* InData)
{
	if (!InData) return;

	// 목록에 추가
	EquippedAccessories.Add(InData);

	// 1. 정적 스탯(이동속도 등) 즉시 적용
	ApplyStaticStats(InData);

	// 2. 동적 효과(OnKill 등) 캐싱
	RebuildEffectCache();
	CachedInventoryComp->TryAddAccessory(InData->ID);
	OnAccUpdated.Broadcast(InData);
	UE_LOG(LogTemp, Error, TEXT("Acc Added"));
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

	// ★ 이제 TArray가 아니라 TMap을 순회합니다! (Key: 데이터, Value: 레벨)
	for (const auto& Pair : EquippedAccessories)
	{
		const UDEAccessoryData* Data = Pair.Key;
		int32 CurrentLevel = Pair.Value; // 현재 악세서리의 레벨 (1, 2, 3...)

		if (!Data) continue;

		for (const FEffectData& BaseEffect : Data->Effects)
		{
			if (BaseEffect.Trigger == EEffectTrigger::None) continue;

			// 1. 원본 데이터를 복사합니다.
			FEffectData ScaledEffect = BaseEffect;

			// 2. ★ 핵심: 레벨에 비례해서 수치를 뻥튀기해 줍니다!
			// (1렙때 10% 였다면, 2렙때는 20%, 5렙때는 50%가 됨)
			ScaledEffect.Chance = BaseEffect.Chance * CurrentLevel;
			ScaledEffect.Value = BaseEffect.Value * CurrentLevel;

			// 만약 지속시간(Duration)이나 다른 수치가 있다면 똑같이 곱해주면 됩니다.
			// ScaledEffect.Duration = BaseEffect.Duration * CurrentLevel;

			// 3. 뻥튀기된 효과를 캐시에 등록!
			CachedTriggerEffects.FindOrAdd(ScaledEffect.Trigger).Add(ScaledEffect);
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
			UE_LOG(LogTemp, Error, TEXT("Acc Stat Added"));
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