// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_BloodySpear.h"
#include "DEBehavior_SelectTargetsInRadius.h"
#include "DEBehavior_PlayEffect.h"
#include "DEBehavior_InstantDamage.h"
#include "DEBehavior_FilterTargets.h"
#include "NiagaraSystem.h"

UDESkill_BloodySpear::UDESkill_BloodySpear()
{
	//static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraRef(TEXT("/Game/DarkEden/Data/Niagara/NS_TalonOfCrow.NS_TalonOfCrow"));
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraRef(TEXT("/Game/DarkEden/Data/Niagara/NS_BloodySpear.NS_BloodySpear"));
	if (NiagaraRef.Succeeded())
	{
		NiagaraFx = NiagaraRef.Object;
		UE_LOG(LogTemp, Warning, TEXT("Bloody spear niagara set"));
	}

	// 1-2. 타격 이펙트 (피격 시)
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundRef(TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/SC_BloodySpear.SC_BloodySpear"));
	if (SoundRef.Succeeded())
	{
		SoundFx = SoundRef.Object;
	}
}


void UDESkill_BloodySpear::InitBehaviors()
{
    Super::InitBehaviors();

	

	UDEBehavior_SelectTargetsInRadius* Targeting = NewObject<UDEBehavior_SelectTargetsInRadius>(this);
	Targeting->Radius = -1.0f;
	Targeting->OriginType = ESearchOrigin::Instigator;
	Behaviors.Add(Targeting);

	UDEBehavior_FilterTargets* Filtering = NewObject<UDEBehavior_FilterTargets>(this);
	Filtering->TargetCount = -1;
	Filtering->FilterType = ETargetFilterType::HighestHP;
	Behaviors.Add(Filtering);

	UDEBehavior_PlayEffect* HitVFX = NewObject<UDEBehavior_PlayEffect>(this, TEXT("HitVFX"));
	if (HitVFX)
	{
		// 시각 효과: 필터링으로 걸러진 "모든 타겟(AllTargets)" 머리 위에 꽂힘!
		HitVFX->NiagaraTargetType = EEffectTargetType::AllTargets;
		HitVFX->NiagaraEffect = NiagaraFx; // 블러디 스피어 떨어지는 나이아가라 에셋

		// 청각 효과: 시전자(Instigator) 기준으로 2D 사운드 1번만 쩌렁쩌렁하게 재생!
		HitVFX->SoundTargetType = EEffectTargetType::Instigator;
		HitVFX->SoundEffect = SoundFx; // 방금 만드신 피 터지는 사운드 큐 에셋!

		Behaviors.Add(HitVFX); // 데미지 들어가기 직전에 꽂아줍니다!
	}

	UDEBehavior_InstantDamage* Damage = NewObject<UDEBehavior_InstantDamage>(this, TEXT("Damage"));
	if (Damage)
	{
		Damage->DamageMultiplier = 1.0f; // 기본 데미지의 100%
		Behaviors.Add(Damage);
	}

}
