// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SelectNearestTarget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "DEMonsterBase.h"
#include "DEMonsterQuerySubsystem.h"
#include "Engine/World.h"
#include "DESkillContext.h"

void UDEBehavior_SelectNearestTarget::Execute(FDESkillContext& Context)
{
	if (!Context.Instigator) return;

	// 1. 기존 타겟 비우기 (새로 찾을 거니까)
	Context.Targets.Empty();

	UWorld* World = Context.Instigator->GetWorld();
	if (!World) return;

	// 2. 탐색 범위 결정 (Context에 'Range'나 'Radius'가 있으면 우선 사용)
	float Radius = Context.GetValue(TEXT("Radius"), DefaultSearchRadius);
	if (Radius <= 0.f) Radius = DefaultSearchRadius;

	const UDEMonsterQuerySubsystem* Query = World->GetSubsystem<UDEMonsterQuerySubsystem>();
	if (!Query) return;

	// 3. 범위 내 가장 가까운 몬스터를 Context에 저장
	if (ADEMonsterBase* Nearest = Query->QueryNearest(Context.Instigator->GetActorLocation(), Radius))
	{
		Context.Targets.Add(Nearest);
	}

}
