// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatEffect_Execution.h"
#include "DEDamageTextSubsystem.h"
#include "DEHealthComponent.h"


void UDECombatEffect_Execution::OnExecuteEffect(FCombatEventData& EventData)
{
    if (!EventData.Target) return;

    UDEHealthComponent* HealthComp = EventData.Target->FindComponentByClass<UDEHealthComponent>();
    // 죽은 놈은 두 번 처형하지 않음
    if (!HealthComp || HealthComp->IsDead()) return;

    float HPPercent = HealthComp->GetCurrentHP() / HealthComp->GetMaxHP();

    // 처형 조건 만족 시 (예: 체력 10% 이하)
    if (HPPercent <= Value)
    {
        // 1. [연출] 텍스트 먼저 띄우기! (몬스터가 사라지기 전에 위치 확보)
        if (UWorld* World = EventData.Target->GetWorld())
        {
            if (UDEDamageTextSubsystem* DmgSys = World->GetSubsystem<UDEDamageTextSubsystem>())
            {
                FDamageVisualInfo DmgInfo;
                // 머리 위쪽으로 띄우기 위해 Z축 살짝 올림
                DmgInfo.WorldLocation = EventData.Target->GetActorLocation() + FVector(0, 0, 100);
                DmgInfo.Amount = HealthComp->GetMaxHP(); // 숫자는 안 보이겠지만 값은 채워줌

                // 어제 세팅해둔 핏빛 EXECUTED Enum 적용!
                DmgInfo.TextType = EDamageTextType::Execution;

                DmgSys->ShowDamage(DmgInfo);
                UE_LOG(LogTemp, Warning, TEXT("EXECUTED ! ! !"));
            }
        }

        // 2. [데이터] 깔끔하게 숨통 끊기 (HealthComp는 UI 신경 안 쓰고 피만 0으로 만듦)
        HealthComp->InstantKill(EventData.Instigator, false);
    }
    //if (!EventData.Target) return;

    ////UE_LOG(LogTemp, Warning, TEXT("On Hit Test . . . "));


    ////original
    //UDEHealthComponent* HealthComp = EventData.Target->FindComponentByClass<UDEHealthComponent>();
    //if (!HealthComp) return;

    //float HPPercent = HealthComp->GetCurrentHP() / HealthComp->GetMaxHP();

    //// 처형 조건 만족 시
    //if (HPPercent <= Threshold)
    //{
    //    // 최대 체력만큼 데미지를 줘서 즉사 처리
    //    HealthComp->InstantKill(EventData.Instigator);
    //}
}
