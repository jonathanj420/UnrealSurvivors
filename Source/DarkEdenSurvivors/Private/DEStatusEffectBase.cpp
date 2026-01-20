// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffectBase.h"
#include "DEMonsterBase.h"

void UDEStatusEffectBase::InitEffect(AActor* InInstigator, ADEMonsterBase* InTarget, float InDuration, float InPower, float InInterval)
{
    Instigator = InInstigator;
    Target = InTarget;
    Duration = InDuration;
    Power = InPower;
    Interval = InInterval;
    ElapsedTime = 0.f;
    TickTimer = 0.f;
}

void UDEStatusEffectBase::OnApply() {} // 자식이 구현
void UDEStatusEffectBase::OnRemove() {} // 자식이 구현

void UDEStatusEffectBase::OnTick(float DeltaTime)
{
    ElapsedTime += DeltaTime;

    // 인터벌이 있는 경우 (도트 데미지 등)
    if (Interval > 0.f)
    {
        TickTimer += DeltaTime;
        if (TickTimer >= Interval)
        {
            TickTimer -= Interval;
            // 여기서 실제 로직은 자식 클래스가 ExecuteTick() 같은 걸 만들어서 처리하거나
            // 그냥 여기서 로직을 넣어도 됨. 편의상 자식이 오버라이드 하기 쉽게 둠.
        }
    }
}