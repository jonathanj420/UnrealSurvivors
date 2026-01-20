// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_ApplyStatusEffect.h"
#include "DEMonsterBase.h"
#include "DESkillContext.h"
#include "DEStatusEffectComponent.h"

void UDEBehavior_ApplyStatusEffect::Execute(FDESkillContext& Context)
{
	if (!StatusClass) return;

	// ★ 핵심: Context에 담겨있는 타겟들에게 상태이상을 겁니다.
	// 즉, 이 Behavior 앞에는 반드시 "타겟을 찾는 Behavior"가 실행되어야 합니다.
	for (AActor* TargetActor : Context.Targets)
	{
		ADEMonsterBase* Monster = Cast<ADEMonsterBase>(TargetActor);
		if (Monster && Monster->StatusEffectComponent)
		{
			Monster->StatusEffectComponent->AddEffect(
				StatusClass,
				Context.Instigator,
				Duration,
				Power,
				Interval
			);
		}
	}
}