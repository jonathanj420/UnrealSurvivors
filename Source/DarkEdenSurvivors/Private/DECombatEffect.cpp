// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatEffect.h"

void UDECombatEffect::ExecuteEffect(FCombatEventData& EventData)
{
    UE_LOG(LogTemp, Error, TEXT("%s ::ExecuteEffect"),*GetName());
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

    if (!World) return false;

    // 1. [도파민] 일단 때릴 때마다 무조건 주사위부터 굴린다! (확률 검사)
    if (FMath::FRand() > Chance)
    {
        return false; // 운이 없어서 안 터짐 (쿨타임 스톱워치는 건드리지도 않음)
    }

    // 2. 쿨타임이 없는 효과면 바로 프리패스 발동!
    if (InternalCooldown <= 0.0f) return true;

    // 3. [밸런스] 주사위는 성공했는데, 혹시 지금 쿨타임 돌고 있는 중인가?
    float CurrentTime = World->GetTimeSeconds();
    if (CurrentTime - LastTriggerTime < InternalCooldown)
    {
        return false; // 아깝다! 주사위 성공했지만 아직 쿨타임이라 무효!
    }

    // 4. [최종 승인] 주사위도 뚫고 쿨타임도 지났다! 
    // 발동 스톱워치를 지금 딱 누르고 진짜 효과를 터뜨린다!
    LastTriggerTime = CurrentTime;
    return true;

    //// 월드가 없거나, 기획자가 쿨타임을 0으로 해뒀으면 프리패스! y no world?
    //if (!World || InternalCooldown <= 0.0f) return true;

    //float CurrentTime = World->GetTimeSeconds();

    //// 아직 쿨이 덜 돌았다면 컷!
    //if (CurrentTime - LastTriggerTime < InternalCooldown)
    //{
    //    return false;
    //}

    //// 통과했으면 마지막 발동 시간 갱신
    //LastTriggerTime = CurrentTime;
    //return true;
}
