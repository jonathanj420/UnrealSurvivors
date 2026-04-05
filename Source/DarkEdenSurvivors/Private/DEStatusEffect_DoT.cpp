// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffect_DoT.h"
#include "DEGameplayLibrary.h"
#include "NiagaraFunctionLibrary.h"

UDEStatusEffect_DoT::UDEStatusEffect_DoT()
{
	StackPolicy = EStackPolicy::Stack;
	MaxStacks = 5;
	//DamageType = EDEDamageType::Poison; // 기본값
}

void UDEStatusEffect_DoT::OnIntervalTick(AActor* Target, FActiveStatusEffect& EffectData) const
{
	if (!Target || !EffectData.Instigator.IsValid()) return;

	// 1. 완벽한 커스텀 데미지 요청(Request) 조립
	FDEDamageRequest DamageReq;
	DamageReq.Instigator = EffectData.Instigator.Get();
	DamageReq.DamageCauser = EffectData.Instigator.Get(); // 혹은 도트 장판 액터가 있다면 그걸 넣어도 됨

	// 소스 오브젝트는 이 상태이상 거푸집(CDO) 자신을 넘겨줍니다. 
	// 나중에 킬로그에서 "독(아이콘)으로 죽음"을 띄울 수 있습니다.
	DamageReq.SourceObject = const_cast<UDEStatusEffect_DoT*>(this);

	DamageReq.Victim = Target;
	DamageReq.BaseDamage = EffectData.Power * EffectData.CurrentStacks;
	DamageReq.DamageTags = DoTDamageTags;


	// ★★★ 극도로 중요한 세팅 ★★★
	// 도트 데미지는 크리티컬이나 흡혈, 온힛(On-Hit) 효과를 터뜨리면 안 됩니다!
	// 만약 독 틱뎀이 온힛을 터뜨리고 -> 그 온힛이 다시 독을 걸면 -> 영원한 무한루프 크래시가 납니다.
	DamageReq.DamageTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Damage.Mechanic.DoT")));
	DamageReq.CritChance = 0.0f;
	DamageReq.LifeStealChance = 0.0f;
	DamageReq.bCanTriggerOnHit = false;

	// 2. 타겟에게 데미지 전달
	FDEDamageResult Result = UDEGameplayLibrary::ApplyCombatDamage(DamageReq);

	// 3. 시각적 피드백 (파티클)
	if (TickParticle)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			Target->GetWorld(),
			TickParticle,
			Target->GetActorLocation()
		);
	}
}
