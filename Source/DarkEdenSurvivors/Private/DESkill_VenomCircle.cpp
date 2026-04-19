// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_VenomCircle.h"
#include "DEBehavior_InstantDamage.h"
#include "DEBehavior_SelectTargetsInRadius.h"
#include "DEBehavior_PlayEffect.h"
#include "DEBehavior_ApplyStatusEffect.h"
#include "NiagaraSystem.h"
#include "DEBehavior_ApplyKnockback.h"


//void UDESkill_VenomCircle::ActivateSkill(FDESkillData* SkillData)
//{
//    
//}

UDESkill_VenomCircle::UDESkill_VenomCircle()
{
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> CastRef(TEXT("/Game/DarkEden/Data/Niagara/NS_VenomCircleMk2.NS_VenomCircleMk2"));
	if (CastRef.Succeeded())
	{
		CastEffectAsset = CastRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("VenomCircle: Failed to load Cast VFX!"));
	}

	// 1-2. sound 이펙트
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundRef(TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/venom_circle.venom_circle"));
	if (SoundRef.Succeeded())
	{
		SoundFx = SoundRef.Object;
	}
}

void UDESkill_VenomCircle::InitBehaviors()
{
	Super::InitBehaviors(); // Behaviors 배열 초기화

	// -------------------------------------------------------------------------
	// [2. Behavior 파이프라인 조립]
	// 순서: 시전VFX -> 타겟탐색 -> 데미지 -> 타격VFX -> 독부여
	// -------------------------------------------------------------------------

	// Step 1. [VFX] 시전 이펙트 재생
	auto* CastVFX = NewObject<UDEBehavior_PlayEffect>(this, TEXT("CastVFX"));
	if (CastVFX)
	{
		CastVFX->TargetType = EEffectTargetType::Instigator; // 시전자 위치
		CastVFX->bAttachToActor = false; // 바닥에 퍼지고 끝남 (따라오지 않음)
		CastVFX->NiagaraEffect = CastEffectAsset; // 위에서 로드한 에셋 연결
		CastVFX->SizeVariableName = TEXT("User.SkillRadius");
		CastVFX->SoundEffect = SoundFx; // 방금 만드신 피 터지는 사운드 큐 에셋!
		Behaviors.Add(CastVFX);
	}

	// Step 2. [Logic] 주변 적 탐색
	auto* Selector = NewObject<UDEBehavior_SelectTargetsInRadius>(this, TEXT("Selector"));
	if (Selector)
	{
		Selector->OriginType = ESearchOrigin::Instigator; // 내 주변
		Selector->bAllowOverlap = false; // 중복 타격 방지 (한 놈당 한 대만)

		// ※ 중요: Radius 값은 여기서 넣지 않습니다. 
		// 실행 시 Context.Radius (데이터 테이블 값)가 자동으로 적용됩니다.

		Behaviors.Add(Selector);
	}

	// Step 3. [Logic] 즉발 데미지 (100%)
	auto* Damage = NewObject<UDEBehavior_InstantDamage>(this, TEXT("Damage"));
	if (Damage)
	{
		Damage->DamageMultiplier = 1.0f; // 기본 데미지의 100%
		Behaviors.Add(Damage);
	}

	auto* Knockback = NewObject<UDEBehavior_ApplyKnockback>(this, TEXT("Knockback"));
	if (Knockback)
	{
		Knockback->ForceMultiplier = 1.0f;
		Behaviors.Add(Knockback);

	}
	//// Step 4. [VFX] 타격 이펙트 (맞은 적들)
	//auto* HitVFX = NewObject<UDEBehavior_PlayNiagara>(this, TEXT("HitVFX"));
	//if (HitVFX)
	//{
	//	HitVFX->SpawnLocation = EEffectSpawnLocation::TargetActors; // 맞은 놈들 위치
	//	HitVFX->Offset = FVector(0.f, 0.f, 60.f); // 가슴 높이 보정
	//	HitVFX->NiagaraAsset = HitEffectAsset;

	//	Behaviors.Add(HitVFX);
	//}

	//// Step 5. [Logic] 독 상태이상 부여
	//auto* Poison = NewObject<UDEBehavior_ApplyStatusEffect>(this, TEXT("Poison"));
	//if (Poison)
	//{
	//	// EDEStatusType 열거형이 정의되어 있어야 함
	//	Poison->StatusType = EDEStatusType::Poison;

	//	// 지속시간, 틱 데미지 등은 Context.CustomValues에서 
	//	// Behavior가 알아서 꺼내 쓰도록 구현 (ApplyStatusEffect 내부 로직)

	//	Behaviors.Add(Poison);
	//}
}
