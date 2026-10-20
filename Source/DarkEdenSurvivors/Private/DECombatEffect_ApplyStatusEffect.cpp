// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatEffect_ApplyStatusEffect.h"
#include "DEStatusEffectComponent.h"
#include "DESkillContext.h"

void UDECombatEffect_ApplyStatusEffect::OnExecuteEffect(FCombatEventData& EventData)
{
    if (!EventData.Target || !EventData.Instigator) return;

    // 타겟의 상태이상 컴포넌트 찾기
    UDEStatusEffectComponent* StatusComp = EventData.Target->FindComponentByClass<UDEStatusEffectComponent>();

    if (StatusComp && StatusEffectClass)
    {
        // ★ 타겟에게 디버프를 겁니다! (Power에 데미지, Duration에 1초 세팅)
        // 블러디 스카리파이 폭발 딜이 원본 낙뢰 딜의 1.5배라면: EventData.DamageAmount * 1.5f
        // SourceContext는 스킬 로컬 이펙트에서만 채워진다. 전역 OnHit/OnKill(장신구, 퍽)에서는 null
        const float Radius = EventData.SourceContext ? EventData.SourceContext->Radius : 0.f;
        StatusComp->AddEffect(StatusEffectClass, EventData.Instigator, Duration, EventData.DamageAmount, Interval, Radius);
    }
}
