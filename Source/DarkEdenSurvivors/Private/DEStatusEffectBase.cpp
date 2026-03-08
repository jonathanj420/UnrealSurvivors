// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffectBase.h"
#include "DEMonsterBase.h"

void UDEStatusEffectBase::InitEffect(AActor* InInstigator, AActor* InTarget, float InDuration, float InPower, float InInterval)
{
	Instigator = InInstigator;
	Target = InTarget;
	Duration = InDuration;
	Power = InPower;
	Interval = InInterval;

	ElapsedTime = 0.f;
	TickTimer = 0.f;
	CurrentStacks = 1; // 처음 걸렸으니 1스택!
}

void UDEStatusEffectBase::Tick(float DeltaTime)
{
	ElapsedTime += DeltaTime;

	// 인터벌이 있는 경우 (도트 데미지 등)
	if (Interval > 0.f)
	{
		TickTimer += DeltaTime;
		if (TickTimer >= Interval)
		{
			TickTimer -= Interval;

			// 도트 딜 로직 발동!
			OnIntervalTick();
		}
	}
}

// BlueprintNativeEvent의 C++ 기본 구현부 (_Implementation을 붙여야 함!)
void UDEStatusEffectBase::OnApply_Implementation()
{
	// 자식 클래스(C++)에서 Super::OnApply_Implementation() 호출 후 로직 작성
}

void UDEStatusEffectBase::OnRemove_Implementation()
{
	// 자식 클래스(C++)에서 로직 작성
}

void UDEStatusEffectBase::OnIntervalTick_Implementation()
{
	// 자식 클래스(C++)에서 로직 작성 (예: ApplyCombatDamage 호출)
}

void UDEStatusEffectBase::OnStacked_Implementation(int32 NewStackCount)
{
	// 자식 클래스(C++)에서 로직 작성
}