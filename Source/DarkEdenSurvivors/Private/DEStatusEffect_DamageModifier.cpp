// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffect_DamageModifier.h"

void UDEStatusEffect_DamageModifier::ModifyIncomingDamage(const FActiveStatusEffect& EffectData, FDEDamageRequest& InOutRequest) const
{
	// 틱뎀(Poison, Bleed)은 증폭시키지 않음 (무한루프 방지)
	static const FGameplayTag Tag_DoT = FGameplayTag::RequestGameplayTag(FName("Damage.Mechanic.DoT"));
	if (InOutRequest.DamageTags.HasTag(Tag_DoT))
	{
		return;
	}

	float TotalModifier = ModifierValuePerStack * EffectData.CurrentStacks;

	switch (ModifierType)
	{
	case EDamageModifierType::Flat:
	{
		// 고정 데미지 증감
		InOutRequest.BaseDamage += TotalModifier;
		InOutRequest.BaseDamage = FMath::Max(0.0f, InOutRequest.BaseDamage);
		break;
	}
	case EDamageModifierType::Percentage:
	{
		// 퍼센트 증폭
		InOutRequest.BaseDamage *= (1.0f + TotalModifier);
		break;
	}
	}
}