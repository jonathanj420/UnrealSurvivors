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
        ExecutionRequest.SourceObject = EventData.SourceObject;                   // ★ 이 '처형' 이펙트 자체를 원인으로 기록! (시너지용)
        ExecutionRequest.Victim = EventData.Target;           // 맞은 놈


        // 2. ★ 가장 중요한 핵심: 데미지 타입을 'Execution'으로 지정! ★
        //ExecutionRequest.DamageType = EDEDamageType::Execution;
        ExecutionRequest.DamageTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Damage.Mechanic.Execution")));
        ExecutionRequest.bCanTriggerOnHit = false;

        // 3. 데미지 수치는 고민하지 마십쇼! 
        // HealthComp 안의 ProcessDamage switch문에서 MaxHP * 100.0f로 덮어씌울 겁니다.
        ExecutionRequest.BaseDamage = 0.0f;

        // 4. 쾅! 명령을 하달함.
        // 이러면 연출, 데이터 처리, 사망 처리, 시너지까지 모든 건 HealthComp가 책임집니다.
        UDEGameplayLibrary::ApplyCombatDamage(ExecutionRequest);

        // UE_LOG(LogTemp, Warning, TEXT("Execution request sent for target: %s"), *EventData.Target->GetName());
    }


}
