// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_InstantDamage.h"
#include "DESkillContext.h"
#include "Kismet/GameplayStatics.h"

void UDEBehavior_InstantDamage::Execute(FDESkillContext& Context)
{
	// Context에서 기본 데미지 가져오기
	float BaseDamage = Context.Damage;
	float FinalDamage = BaseDamage * DamageMultiplier;

	// 타겟 목록 순회하며 데미지
	for (AActor* Target : Context.Targets)
	{
		if (Target && !Target->IsPendingKillPending())
		{
			UGameplayStatics::ApplyDamage(
				Target,
				FinalDamage,
				Context.Instigator->GetInstigatorController(),
				Context.Instigator,
				UDamageType::StaticClass()
			);
		}
	}

}
