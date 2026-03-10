// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatEffect.h"

void UDECombatEffect::ExecuteEffect(FCombatEventData& EventData)
{
    if (!EventData.Instigator) return;

    // 1. 부모가 강제로 쿨타임 검문소 검사! (자식들은 이제 이거 신경 쓸 필요 없음)
    if (!CanTriggerEffect(EventData.Instigator->GetWorld()))
    {
        return; // 통과 못 하면 여기서 컷!
    }

    // 2. 검문소를 무사히 통과했다면? 자식들이 각자 짜둔 진짜 효과 발동!
    OnExecuteEffect(EventData);
}

bool UDECombatEffect::CanTriggerEffect(UWorld* World)
{
    // 월드가 없거나, 기획자가 쿨타임을 0으로 해뒀으면 프리패스!
    if (!World || InternalCooldown <= 0.0f) return true;

    float CurrentTime = World->GetTimeSeconds();

    // 아직 쿨이 덜 돌았다면 컷!
    if (CurrentTime - LastTriggerTime < InternalCooldown)
    {
        return false;
    }

    // 통과했으면 마지막 발동 시간 갱신
    LastTriggerTime = CurrentTime;
    return true;
}
