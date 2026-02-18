// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UDEStatComponent::UDEStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// =========================================================
	// [1] Combat Stats (전투 스탯) - ★ 여기가 0이라 문제였음!
	// =========================================================

	// 곱연산(Multiplier)으로 쓰이는 애들은 무조건 1.0f가 기본값이어야 함
	DamageMultiplier = FGameplayStat(1.0f); // 공격력 100%
	CritChance = FGameplayStat(0.0f); // 기본 크리 5% (취향껏 조절)
	CritDamageMultiplier = FGameplayStat(2.0f); // 크리 데미지 200%

	CooldownReduction = FGameplayStat(0.0f); // 쿨감 0%

	AreaSize = FGameplayStat(1.0f); // 범위 100%
	Duration = FGameplayStat(1.0f); // 지속시간 100%
	ProjectileSpeed = FGameplayStat(1.0f); // 투사체 속도 100%

	BonusAmount = FGameplayStat(0.0f); // 추가 투사체 0개 (이건 더하기니까 0이 맞음)


	// =========================================================
	// [2] Physical Stats (신체 능력)
	// =========================================================
	MoveSpeed = FGameplayStat(600.0f); // 기본 이속 600
	MagnetRange = FGameplayStat(200.0f); // 자석 범위 좀 넉넉하게
	MaxHP = FGameplayStat(100.0f); // 체력 100
	Regeneration = FGameplayStat(0.0f);   // 재생 0
	Armor = FGameplayStat(0.0f);   // 방어 0


	// =========================================================
	// [3] Utility Stats (유틸리티)
	// =========================================================
	Luck = FGameplayStat(1.0f);
	Greed = FGameplayStat(1.0f);
	Growth = FGameplayStat(1.0f);
	Curse = FGameplayStat(1.0f);
	Revival = FGameplayStat(0.0f);
}


// Called when the game starts
void UDEStatComponent::BeginPlay()
{
	Super::BeginPlay();

	//  1. 레지스트리 등록 (Enum -> 변수 포인터 매핑)
	// 이렇게 해두면 나중에 반복문이나 검색으로 바로 접근 가능!
	StatRegistry.Add(EDEStatType::Damage, &DamageMultiplier);
	StatRegistry.Add(EDEStatType::CritChance, &CritChance);
	StatRegistry.Add(EDEStatType::CritDamage, &CritDamageMultiplier);
	StatRegistry.Add(EDEStatType::Cooldown, &CooldownReduction);
	StatRegistry.Add(EDEStatType::Area, &AreaSize);
	StatRegistry.Add(EDEStatType::Duration, &Duration);
	StatRegistry.Add(EDEStatType::ProjectileSpeed, &ProjectileSpeed);
	StatRegistry.Add(EDEStatType::Amount, &BonusAmount);

	StatRegistry.Add(EDEStatType::MoveSpeed, &MoveSpeed);
	StatRegistry.Add(EDEStatType::Magnet, &MagnetRange);
	StatRegistry.Add(EDEStatType::MaxHP, &MaxHP);
	StatRegistry.Add(EDEStatType::Regeneration, &Regeneration);
	StatRegistry.Add(EDEStatType::Armor, &Armor);

	StatRegistry.Add(EDEStatType::Luck, &Luck);
	StatRegistry.Add(EDEStatType::Greed, &Greed);
	StatRegistry.Add(EDEStatType::Growth, &Growth);
	StatRegistry.Add(EDEStatType::Curse, &Curse);
	StatRegistry.Add(EDEStatType::Revival, &Revival);
	
}


// Called every frame
void UDEStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UDEStatComponent::ApplyModifier(const FDEStatModifier& Mod)
{
	// 1. 맵에서 해당 스탯 변수 찾기 (O(1))
	if (FGameplayStat** FoundStat = StatRegistry.Find(Mod.StatType))
	{
		// 2. 값 적용
		(*FoundStat)->ApplyModifier(Mod);

		// 3. 후처리 (이속 등 즉시 반영이 필요한 경우)
		RefreshDerivedStats(Mod.StatType);
	}
	else
	{
		// 예외 처리 (여기 없는 스탯이거나 잘못된 Enum)
		UE_LOG(LogTemp, Warning, TEXT("StatType %d not found in Registry!"), (int32)Mod.StatType);
	}
}

void UDEStatComponent::RefreshDerivedStats(EDEStatType StatType)
{
	// 값이 바뀌었을 때 엔진에 반영해야 하는 것들만 처리
	switch (StatType)
	{
	case EDEStatType::MoveSpeed:
		if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
		{
			if (auto* MoveComp = OwnerChar->GetCharacterMovement())
			{
				MoveComp->MaxWalkSpeed = MoveSpeed.GetValue();
				OnSpeedChanged.Broadcast(MoveComp->MaxWalkSpeed);
			}
		}
		break;

	case EDEStatType::Magnet:
		OnMagnetChanged.Broadcast(MagnetRange.GetValue());
		break;

		// MaxHP 변경 시 현재 체력 비율 유지 로직 등은 HealthComponent와 연동 필요
	}
}

void UDEStatComponent::ResetStats()
{
	for (auto& Pair : StatRegistry)
	{
		if (Pair.Value)
		{
			Pair.Value->ResetModifiers();
		}
	}
	// 초기화 후 반영
	RefreshDerivedStats(EDEStatType::MoveSpeed);
}