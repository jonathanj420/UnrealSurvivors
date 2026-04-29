// Fill out your copyright notice in the Description page of Project Settings.

#include "DEStatComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Components/SphereComponent.h"
#include "DECharacterBase.h"
#include "DEGameInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UDEStatComponent::UDEStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 기본 능력치 초기화
	DamageMultiplier = FGameplayStat(1.0f);
	CritChance = FGameplayStat(0.0f);
	CritDamageMultiplier = FGameplayStat(2.0f);
	CooldownReduction = FGameplayStat(0.0f);
	AreaSize = FGameplayStat(1.0f);
	Duration = FGameplayStat(1.0f);
	ProjectileSpeed = FGameplayStat(1.0f);
	BonusAmount = FGameplayStat(0.0f);

	MoveSpeed = FGameplayStat(600.0f);
	MagnetRange = FGameplayStat(200.0f);
	MaxHP = FGameplayStat(100.0f);
	Regeneration = FGameplayStat(0.0f);
	Armor = FGameplayStat(0.0f);

	Luck = FGameplayStat(1.0f);
	Greed = FGameplayStat(1.0f);
	Growth = FGameplayStat(1.0f);
	Curse = FGameplayStat(1.0f);
	Revival = FGameplayStat(0.0f);
}

void UDEStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// Enum 매핑 등록
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

void UDEStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UE_LOG(LogTemp, Warning, TEXT("y t fuck tick on?"));
}

void UDEStatComponent::ApplyModifier(const FDEStatModifier& Mod)
{
	if (FGameplayStat** FoundStat = StatRegistry.Find(Mod.StatType))
	{
		(*FoundStat)->ApplyModifier(Mod);
		RefreshDerivedStats(Mod.StatType);
		OnStatChanged.Broadcast(Mod.StatType, (*FoundStat)->GetValue()); // ← 추가
	}
}

void UDEStatComponent::RefreshDerivedStats(EDEStatType StatType)
{
	switch (StatType)
	{
	case EDEStatType::MoveSpeed:
		if (OwnedChar && OwnedChar->GetCharacterMovement())
		{
			OwnedChar->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed.GetValue();
			OnSpeedChanged.Broadcast(OwnedChar->GetCharacterMovement()->MaxWalkSpeed);
			UE_LOG(LogTemp, Warning, TEXT("Movespeed Changed : %f"), MoveSpeed.GetValue());
		}
		break;

	case EDEStatType::Magnet:
		OnMagnetChanged.Broadcast(MagnetRange.GetValue());
		break;

	case EDEStatType::MaxHP:
		if (OwnedChar)
		{
			OwnedChar->SetMaxHP(MaxHP.GetValue());
		}
		break;
	}
}

void UDEStatComponent::InitAsPlayer(ADECharacterBase* InPlayer)
{
	OwnedChar = InPlayer;
    
    // 영구 강화 수치 적용
    ApplyMetaUpgrades();

	RefreshDerivedStats(EDEStatType::MoveSpeed);
	RefreshDerivedStats(EDEStatType::MaxHP);
	RefreshDerivedStats(EDEStatType::Magnet);
}

void UDEStatComponent::ApplyMetaUpgrades()
{
    UDEGameInstance* GI = Cast<UDEGameInstance>(GetWorld()->GetGameInstance());
    if (!GI) return;

    // 적용할 스태트 목록
    TArray<EDEStatType> MetaStats = { 
        EDEStatType::Damage, 
        EDEStatType::MaxHP, 
        EDEStatType::MoveSpeed, 
        EDEStatType::Greed, 
        EDEStatType::Amount 
    };

    for (EDEStatType Stat : MetaStats)
    {
        float BonusValue = GI->GetStatUpgradeBonus(Stat);
        if (BonusValue <= 0.0f) continue;

        FDEStatModifier MetaMod;
        MetaMod.StatType = Stat;

        // Amount(발사체 수)는 가산(Additive), 나머지는 승산(Multiplier)으로 처리
        if (Stat == EDEStatType::Amount)
        {
            MetaMod.Additive = BonusValue;
            //MetaMod.Multiplier = 1.0f;
        }
        else
        {
            MetaMod.Additive = 0.0f;
            MetaMod.Multiplier = BonusValue;
        }

        ApplyModifier(MetaMod);
    }
}

float UDEStatComponent::GetStatValue(EDEStatType StatType) const
{
	if (const FGameplayStat* const* FoundStat = StatRegistry.Find(StatType))
	{
		return (*FoundStat)->GetValue();
	}
	return 0.0f;
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
	RefreshDerivedStats(EDEStatType::MoveSpeed);
}
