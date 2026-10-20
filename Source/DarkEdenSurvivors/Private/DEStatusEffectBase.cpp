// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffectBase.h"
#include "DarkEdenSurvivors.h"

UDEStatusEffectBase::UDEStatusEffectBase()
{
	// 기본값 안전 세팅
	StackPolicy = EStackPolicy::Ignore;
	MaxStacks = 1;
}

void UDEStatusEffectBase::Tick(AActor* Target, FActiveStatusEffect& EffectData, float DeltaTime) const
{
	// 1. 전체 지속 시간 업데이트
	EffectData.ElapsedTime += DeltaTime;

	// 2. 인터벌(틱 주기)이 있는 경우 도트 딜 타이머 업데이트
	if (EffectData.Interval > 0.f)
	{
		EffectData.TickTimer += DeltaTime;
		if (EffectData.TickTimer >= EffectData.Interval)
		{
			EffectData.TickTimer -= EffectData.Interval;

			// 마지막에 부른다. 그 안의 데미지가 같은 대상에 AddEffect하면 배열이 재할당돼 EffectData 참조가 깨진다
			OnIntervalTick(Target, EffectData);
		}
	}
}

void UDEStatusEffectBase::OnApply(AActor* Target, FActiveStatusEffect& EffectData) const
{
	UE_LOG(LogDarkEdenSurvivors, Verbose, TEXT("Status Effect : %s On Apply"), *GetName());
	// 자식 클래스(UDEStatusEffect_CC 등)에서 Super::OnApply_Implementation() 호출 후 로직 작성
}

void UDEStatusEffectBase::OnRemove(AActor* Target, FActiveStatusEffect& EffectData) const
{
	UE_LOG(LogDarkEdenSurvivors, Verbose, TEXT("Status Effect : %s On Remove"), *GetName());
	// 자식 클래스에서 스탯 원복 등의 로직 작성
}

void UDEStatusEffectBase::OnIntervalTick(AActor* Target, FActiveStatusEffect& EffectData) const
{
	// UDEStatusEffect_DoT 클래스에서 데미지를 가하는 로직 구현
}

void UDEStatusEffectBase::OnStacked(AActor* Target, FActiveStatusEffect& EffectData, int32 NewStackCount) const
{
	// 스택이 쌓일 때 파티클 펑 터지는 연출이나 사운드 재생
}