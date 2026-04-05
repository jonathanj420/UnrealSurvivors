// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEStatusEffectBase.h"
#include "DEDamageTypes.h"
#include "DEStatusEffect_DamageModifier.generated.h"

UENUM(BlueprintType)
enum class EDamageModifierType : uint8
{
	Flat		UMETA(DisplayName = "Flat + Damage"),
	Percentage	UMETA(DisplayName = "% Damage")
};

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEStatusEffect_DamageModifier : public UDEStatusEffectBase
{
	GENERATED_BODY()

public:
	UDEStatusEffect_DamageModifier()
	{
		// 모디파이어는 보통 틱(Tick) 연산이 아예 필요 없으므로 꺼버려도 됩니다.
	}

	UPROPERTY(EditDefaultsOnly, Category = "Status Effect|Modifier")
	EDamageModifierType ModifierType = EDamageModifierType::Percentage;

	// 마이너스 값(-0.5)을 넣으면 데미지 감소 버프로도 작동!
	UPROPERTY(EditDefaultsOnly, Category = "Status Effect|Modifier")
	float ModifierValuePerStack = 0.05f;

	// Base에 만들어둔 가상 함수 오버라이드
	virtual void ModifyIncomingDamage(const FActiveStatusEffect& EffectData, FDEDamageRequest& InOutRequest) const override;
	
};
