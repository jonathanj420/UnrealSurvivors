// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEStatusEffectBase.h"
#include "DEStatusEffect_DelayedAction.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEStatusEffect_DelayedAction : public UDEStatusEffectBase
{
    GENERATED_BODY()

public:
    UDEStatusEffect_DelayedAction();

    // 디버프 해제 시점(시간 만료 or 몬스터 사망)에 무조건 호출됨
    virtual void OnRemove(AActor* Target, FActiveStatusEffect& EffectData) const override;

protected:
    // ★ 핵심: 자식들이 무조건 구현해야 하는 '진짜 행동' 가상 함수
    virtual void ExecuteAction(AActor* Target, FActiveStatusEffect& EffectData) const PURE_VIRTUAL(UDEStatusEffect_DelayedAction::ExecuteAction, );
	
};
