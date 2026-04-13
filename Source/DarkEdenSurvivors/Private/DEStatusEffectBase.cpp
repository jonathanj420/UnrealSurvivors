// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffectBase.h"
#include "DEMonsterBase.h"
#include "Components/SkeletalMeshComponent.h"    // ★ 몬스터 메쉬를 찾기 위해 필수!
#include "Materials/MaterialInstanceDynamic.h"

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

			// 도트 딜이나 주기적 효과 발동! 
			// (블루프린트나 자식 C++ 클래스에서 오버라이드한 로직이 실행됨)
			OnIntervalTick(Target, EffectData);
		}
	}
}

void UDEStatusEffectBase::OnApply(AActor* Target, FActiveStatusEffect& EffectData) const
{
	UE_LOG(LogTemp, Warning, TEXT("Status Effect : %s On Apply !"), *GetName());
	// 자식 클래스(UDEStatusEffect_CC 등)에서 Super::OnApply_Implementation() 호출 후 로직 작성
	// (예: 여기서 EffectParticle을 Target에 Attach 시키는 공통 로직을 넣기 좋습니다.)
}

void UDEStatusEffectBase::OnRemove(AActor* Target, FActiveStatusEffect& EffectData) const
{
	UE_LOG(LogTemp, Warning, TEXT("Status Effect : %s On Remove !"), *GetName());
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