// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DECombatTypes.h"
#include "DECombatEffect.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class DARKEDENSURVIVORS_API UDECombatEffect : public UObject
{
    GENERATED_BODY()

public:
    // 기획자가 이 효과가 '언제' 터질지 블루프린트에서 고르는 명찰!
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
    ECombatEventTrigger TriggerCondition = ECombatEventTrigger::OnHit;

    // 자식 클래스들이 무조건 구현해야 하는 실제 실행 로직
    // 1. 외부(스킬 등)에서 호출하는 진짜 입구! (더 이상 virtual이 아님!)
    void ExecuteEffect(FCombatEventData& EventData);
protected:
    // =========================================================
    // ★ 2. 자식 클래스들이 '무조건' 오버라이드해야 하는 실제 내부 로직!
    // =========================================================
    virtual void OnExecuteEffect(FCombatEventData& EventData) PURE_VIRTUAL(UDECombatEffect::OnExecuteEffect, );
	
public:
    // ★ 모든 이펙트가 공통으로 가지는 내부 쿨타임! (0이면 제한 없음)
    UPROPERTY(EditAnywhere, Category = "Trigger")
    float InternalCooldown = 0.0f;

private:
    // 마지막 발동 시간
    float LastTriggerTime = -FLT_MAX;

public:
    // 자식 클래스들이 ExecuteEffect 안에서 호출할 '쿨타임 검문소' 함수
    bool CanTriggerEffect(UWorld* World);
};
