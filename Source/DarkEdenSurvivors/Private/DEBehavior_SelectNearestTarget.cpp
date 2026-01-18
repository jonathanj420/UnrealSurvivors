// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SelectNearestTarget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "DEMonsterBase.h"
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

	FVector MyLoc = Context.Instigator->GetActorLocation();

	// 3. 범위 내 오버랩 검사 (구체)
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Context.Instigator); // 나 자신 제외

	// Enemy 채널이나 Pawn 채널로 검사 (프로젝트 설정에 따라 수정 필요)
	// 여기서는 ECC_Pawn으로 검사하고 태그를 확인하는 범용 방식을 씀
	bool bHit = World->OverlapMultiByChannel(
		Overlaps,
		MyLoc,
		FQuat::Identity,
		ECollisionChannel::ECC_WorldDynamic,
		FCollisionShape::MakeSphere(Radius),
		QueryParams
	);

	AActor* NearestActor = nullptr;
	float MinDistSq = FLT_MAX;

	// 4. 가장 가까운 놈 찾기
	if (bHit)
	{
		for (const FOverlapResult& Res : Overlaps)
		{
			AActor* Actor = Res.GetActor();
			// 살아있고 + 적 태그가 있는지 확인
			if (Actor && Actor->ActorHasTag(EnemyTag))
			{
				float DistSq = FVector::DistSquared(MyLoc, Actor->GetActorLocation());
				if (DistSq < MinDistSq)
				{
					MinDistSq = DistSq;
					NearestActor = Actor;
				}
			}
		}
	}

	// 5. 찾았으면 Context에 저장
	if (NearestActor)
	{
		Context.Targets.Add(NearestActor);
		// UE_LOG(LogTemp, Log, TEXT("Target Found: %s"), *NearestActor->GetName());
	}

}
