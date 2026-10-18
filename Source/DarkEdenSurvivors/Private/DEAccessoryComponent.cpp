// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAccessoryComponent.h"
#include "DEStatComponent.h"
#include "DEInventoryComponent.h"
#include "DECombatComponent.h"
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

		//  3. 구시대의 유물(구조체 캐싱)은 삭제!

		// 4. [NEW] CombatComponent 사령관에게 전설 기믹 장착/스택업 지시!
		if (UDECombatComponent* CombatComp = GetOwner()->FindComponentByClass<UDECombatComponent>())
		{
			// 데이터 에셋에 들어있는 이펙트 클래스들을 쫙 뽑아서 사령관한테 던져줍니다
			for (UDECombatEffect* TemplateEffect : InData->AccessoryEffects)
			{
				// 사령관이 알아서 "어? 새거네 장착!", "어? 있던거네 스택업!" 다 해줍니다.
				CombatComp->AddCombatEffect(TemplateEffect);
			}
		}

		UE_LOG(LogTemp, Warning, TEXT("Accessory %s -> Lv.%d"), *InData->Name.ToString(), CurrentLevel);

		CachedInventoryComp->TryAddAccessory(InData->ID);

		// 5. UI 갱신 알람 발송!
		OnAccUpdated.Broadcast(InData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Accessory] %s is already Max Level (%d)!"), *InData->Name.ToString(), InData->MaxLevel);
	}

}

void UDEAccessoryComponent::RemoveAccessory(const UDEAccessoryData* InData)
{
	if (!InData) return;

	if (EquippedAccessories.Remove(InData) > 0) // 하나라도 지워졌다면
	{
		// 정적 스탯 제거
		RemoveStaticStats(InData);

		// 캐시 다시 굽기
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
}
