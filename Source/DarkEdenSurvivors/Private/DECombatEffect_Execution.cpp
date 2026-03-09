// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatEffect_Execution.h"
#include "DEHealthComponent.h"


void UDECombatEffect_Execution::ExecuteEffect(const FCombatEventData& EventData)
{
    if (!EventData.Target) return;



    UE_LOG(LogTemp, Warning, TEXT("On Hit Test . . . "));


    //original
    //UDEHealthComponent* HealthComp = EventData.Target->FindComponentByClass<UDEHealthComponent>();
    //if (!HealthComp) return;

    //float HPPercent = HealthComp->GetCurrentHP() / HealthComp->GetMaxHP();

    //// 처형 조건 만족 시
    //if (HPPercent <= ThresholdRatio)
    //{
    //    // 최대 체력만큼 데미지를 줘서 즉사 처리
    //    HealthComp->InstantKill(EventData.Instigator);
    //}
}
