// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatEffect_Execution.h"
#include "DEDamageTypes.h"
#include "DEHealthComponent.h"
#include "DEGameplayLibrary.h"


void UDECombatEffect_Execution::OnExecuteEffect(FCombatEventData& EventData)
{
    if (!EventData.Target) return;

    UDEHealthComponent* HealthComp = EventData.Target->FindComponentByClass<UDEHealthComponent>();
    // 죽은 놈은 두 번 처형하지 않음
    if (!HealthComp || HealthComp->IsDead()) return;

    // 현재 체력 퍼센트 계산
    const float HPPercent = HealthComp->GetCurrentHP() / HealthComp->GetMaxHP();

    // 처형 조건 만족 시 (예: 체력 10% 이하)
    if (HPPercent <= Value) // Threshold 대신 우리가 코드로 깎아둔 'Value' 변수 사용
    {
        UE_LOG(LogTemp, Warning, TEXT("EXECUTED ! ! !"));
        // =========================================================
        // ★ 미친 듯이 깔끔해진 AAA급 '처형' 요청!
        // =========================================================
        FDEDamageRequest ExecutionRequest;

        // 1. 누가, 무엇으로 때리는지 정보 세팅 (쿨감 버그 해결!)
        ExecutionRequest.Instigator = EventData.Instigator;   // 공격자 (플레이어)
        ExecutionRequest.DamageCauser = EventData.Instigator; // 공격 도구 (투사체/칼 등)
        ExecutionRequest.SourceObject = this;                   // ★ 이 '처형' 이펙트 자체를 원인으로 기록! (시너지용)
        ExecutionRequest.Victim = EventData.Target;           // 맞은 놈

        // 2. ★ 가장 중요한 핵심: 데미지 타입을 'Execution'으로 지정! ★
        ExecutionRequest.DamageType = EDEDamageType::Execution;

        // 3. 데미지 수치는 고민하지 마십쇼! 
        // HealthComp 안의 ProcessDamage switch문에서 MaxHP * 100.0f로 덮어씌울 겁니다.
        ExecutionRequest.BaseDamage = 0.0f;

        // 4. 쾅! 명령을 하달함.
        // 이러면 연출, 데이터 처리, 사망 처리, 시너지까지 모든 건 HealthComp가 책임집니다.
        UDEGameplayLibrary::ApplyCombatDamage(ExecutionRequest);

        // UE_LOG(LogTemp, Warning, TEXT("Execution request sent for target: %s"), *EventData.Target->GetName());
    }

    //if (!EventData.Target) return;

    //UDEHealthComponent* HealthComp = EventData.Target->FindComponentByClass<UDEHealthComponent>();
    //// 죽은 놈은 두 번 처형하지 않음
    //if (!HealthComp || HealthComp->IsDead()) return;

    //float HPPercent = HealthComp->GetCurrentHP() / HealthComp->GetMaxHP();

    //// 처형 조건 만족 시 (예: 체력 10% 이하)
    //if (HPPercent <= Value)
    //{
    //    // 1. [연출] 텍스트 먼저 띄우기! (몬스터가 사라지기 전에 위치 확보)
    //    if (UWorld* World = EventData.Target->GetWorld())
    //    {
    //        if (UDEDamageTextSubsystem* DmgSys = World->GetSubsystem<UDEDamageTextSubsystem>())
    //        {
    //            FDamageVisualInfo DmgInfo;
    //            // 머리 위쪽으로 띄우기 위해 Z축 살짝 올림
    //            DmgInfo.WorldLocation = EventData.Target->GetActorLocation() + FVector(0, 0, 100);
    //            DmgInfo.Amount = HealthComp->GetMaxHP(); // 숫자는 안 보이겠지만 값은 채워줌

    //            // 어제 세팅해둔 핏빛 EXECUTED Enum 적용!
    //            DmgInfo.TextType = EDamageTextType::Execution;

    //            DmgSys->ShowDamage(DmgInfo);
    //            UE_LOG(LogTemp, Warning, TEXT("EXECUTED ! ! !"));
    //        }
    //    }

    //    // 2. [데이터] 깔끔하게 숨통 끊기 (HealthComp는 UI 신경 안 쓰고 피만 0으로 만듦)
    //    HealthComp->InstantKill(EventData.Instigator, false);
    //}
    // 
    // 
    // 
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
