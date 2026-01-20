// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DEStatusEffectBase.generated.h"

class ADEMonsterBase;
/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEStatusEffectBase : public UObject
{
    GENERATED_BODY()

public:
    // 초기화 (누가, 누구에게, 얼마나, 강도는?)
    virtual void InitEffect(AActor* InInstigator, ADEMonsterBase* InTarget, float InDuration, float InPower, float InInterval = 0.f);

    // 시작될 때 (예: 스턴 걸림 -> 움직임 멈춤)
    virtual void OnApply();

    // 끝날 때 (예: 스턴 풀림 -> 움직임 재개)
    virtual void OnRemove();

    // 틱마다 (예: 독 데미지)
    virtual void OnTick(float DeltaTime);

    bool IsFinished() const { return Duration > 0.f && ElapsedTime >= Duration; }

public:
    UPROPERTY()
    AActor* Instigator; // 시전한 사람

    UPROPERTY()
    ADEMonsterBase* Target; // 당한 몬스터

    float Duration;     // 지속 시간
    float ElapsedTime;  // 경과 시간
    float Power;        // 위력 (데미지 양 or 슬로우 비율)
    float Interval;     // 틱 간격 (도트딜용)
    float TickTimer;    // 틱 계산용
};
