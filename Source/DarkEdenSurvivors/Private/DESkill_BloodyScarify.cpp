// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_BloodyScarify.h"
#include "DEBehavior_SelectTargetsInRadius.h"
#include "DEBehavior_PlayEffect.h"
#include "DEBehavior_InstantDamage.h"
#include "DEBehavior_FilterTargets.h"
#include "NiagaraSystem.h"
#include "DECombatEffect_ApplyStatusEffect.h"
#include "DEStatusEffectBase.h"

UDESkill_BloodyScarify::UDESkill_BloodyScarify()
{
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraRef(TEXT("/Game/DarkEden/Data/Niagara/NS_BloodyScarify.NS_BloodyScarify"));
	//static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraRef(TEXT("/Game/DarkEden/Data/Niagara/NS_BloodySpear.NS_BloodySpear"));
	if (NiagaraRef.Succeeded())
	{
		NiagaraFx = NiagaraRef.Object;
		UE_LOG(LogTemp, Warning, TEXT("Bloody spear niagara set"));
	}

	// 1-2. sound 이펙트
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundRef(TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/SC_BloodySpear.SC_BloodySpear"));
	if (SoundRef.Succeeded())
	{
		SoundFx = SoundRef.Object;
	}

	static ConstructorHelpers::FClassFinder<UDEStatusEffectBase> StatusEffectBP(TEXT("/Game/DarkEden/Blueprint/StatusEffects/BP_StatusEffect_BloodyScarifyExplosion.BP_StatusEffect_BloodyScarifyExplosion_C"));

	// 2. 에셋을 성공적으로 찾았는지 확인 후 변수에 할당
	if (StatusEffectBP.Succeeded())
	{
		// SummonClass 는 보통 헤더에 TSubclassOf<AActor>로 선언합니다.
		StatusEffectClass = StatusEffectBP.Class;
	}

}

void UDESkill_BloodyScarify::InitBehaviors()
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

	UDECombatEffect_ApplyStatusEffect* ApplyStatusEffect = NewObject<UDECombatEffect_ApplyStatusEffect>(this);
	ApplyStatusEffect->TriggerCondition = ECombatEventTrigger::OnHit;

	// ★ 핵심: 하드코딩 10 대신, 블루프린트에서 세팅한 내 ID를 동적으로 가져옴!
	ApplyStatusEffect->StatusEffectClass = StatusEffectClass;
	ApplyStatusEffect->Interval = 0.0f;
	ApplyStatusEffect->Duration = 100.0f;
	//CooldownResetEffect->bInstantReset = true;

	LocalEffects.Add(ApplyStatusEffect);

}
