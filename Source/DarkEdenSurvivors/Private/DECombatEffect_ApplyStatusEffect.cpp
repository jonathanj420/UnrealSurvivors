// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatEffect_ApplyStatusEffect.h"
#include "DEStatusEffectComponent.h"

void UDECombatEffect_ApplyStatusEffect::OnExecuteEffect(FCombatEventData& EventData)
{
    if (!EventData.Target || !EventData.Instigator) return;

    // 타겟의 상태이상 컴포넌트 찾기
    UDEStatusEffectComponent* StatusComp = EventData.Target->FindComponentByClass<UDEStatusEffectComponent>();

    if (StatusComp && StatusEffectClass)
    {
        // ★ 타겟에게 디버프를 겁니다! (Power에 데미지, Duration에 1초 세팅)
        // 블러디 스카리파이 폭발 딜이 원본 낙뢰 딜의 1.5배라면: EventData.DamageAmount * 1.5f
        StatusComp->AddEffect(StatusEffectClass, EventData.Instigator, Duration, EventData.DamageAmount,Interval,*EventData.SourceContext);
    }
}
