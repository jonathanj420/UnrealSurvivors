// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffect_Stun.h"
#include "DEMonsterBase.h"

UDEStatusEffect_Stun::UDEStatusEffect_Stun()
{
	// 이미 걸려 있으면 남은 시간과 새 시간 중 긴 쪽으로 늘린다 (Ignore면 나중에 건 긴 스턴이 무시된다)
	StackPolicy = EStackPolicy::Refresh;
}

void UDEStatusEffect_Stun::OnApply(AActor* Target, FActiveStatusEffect& EffectData) const
{
    // 부모 로직이 있다면 먼저 실행 (현재는 비어있겠지만 관례상 호출)
    Super::OnApply(Target, EffectData);

    if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Target))
    {
        // 1. 매니저가 관리하는 이동/추적 로직 정지
        Monster->AddMoveBlock();
    }
}

void UDEStatusEffect_Stun::OnRemove(AActor* Target, FActiveStatusEffect& EffectData) const
{
    Super::OnRemove(Target, EffectData);

    if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Target))
    {
        // 1. 스턴 해제. 다른 이동불가 효과가 남아 있으면 계속 멈춰 있다
        Monster->RemoveMoveBlock();
    }
}
