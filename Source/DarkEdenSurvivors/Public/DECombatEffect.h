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
    virtual void ExecuteEffect(const FCombatEventData& EventData) PURE_VIRTUAL(UDECombatEffect::ExecuteEffect, );
	
};
