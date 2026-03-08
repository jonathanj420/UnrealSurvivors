// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_FilterTargets.h"
#include "DEMonsterBase.h"
#include "DESkillContext.h"

UDEBehavior_FilterTargets::UDEBehavior_FilterTargets()
{
	TargetCount = 1;
	FilterType = ETargetFilterType::Random;
}

void UDEBehavior_FilterTargets::Execute(FDESkillContext& Context)
{
	// 1. 예외 처리: 타겟이 없거나, 이미 목표보다 적으면 필터링할 필요 없음
	if (Context.Targets.Num() <= TargetCount)
	{
		return;
	}

	// 거리 계산을 위해 내 위치 가져오기
	FVector MyLoc = FVector::ZeroVector;
	if (Context.Instigator)
	{
		MyLoc = Context.Instigator->GetActorLocation();
	}

	// 2. 필터 타입에 따라 정렬(Sorting) 또는 섞기(Shuffle)
	switch (FilterType)
	{
	case ETargetFilterType::Random:
	{
		// Fisher-Yates Shuffle (가장 공평하고 빠른 랜덤 섞기)
		int32 LastIndex = Context.Targets.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			int32 Index = FMath::RandRange(i, LastIndex);
			if (i != Index)
			{
				Context.Targets.Swap(i, Index);
			}
		}
	}
	break;

	case ETargetFilterType::Nearest:
	{
		// 거리 오름차순 (가까운 놈이 앞)
		// 최적화: DistSquared 사용 (루트 연산 제거)
		Context.Targets.Sort([MyLoc](const AActor& A, const AActor& B) {
			return FVector::DistSquared(A.GetActorLocation(), MyLoc) < FVector::DistSquared(B.GetActorLocation(), MyLoc);
			});
	}
	break;

	case ETargetFilterType::Farthest:
	{
		// 거리 내림차순 (먼 놈이 앞)
		Context.Targets.Sort([MyLoc](const AActor& A, const AActor& B) {
			return FVector::DistSquared(A.GetActorLocation(), MyLoc) > FVector::DistSquared(B.GetActorLocation(), MyLoc);
			});
	}
	break;

	case ETargetFilterType::LowestHP:
	{
		// 체력 오름차순 (낮은 놈이 앞)
		Context.Targets.Sort([](const AActor& A, const AActor& B) {
			const auto* MonsterA = Cast<ADEMonsterBase>(&A);
			const auto* MonsterB = Cast<ADEMonsterBase>(&B);

			// 둘 다 몬스터면 체력 비교
			if (MonsterA && MonsterB)
			{
				return MonsterA->GetCurrentHP() < MonsterB->GetCurrentHP();
			}
			// 몬스터가 아니면(혹시 섞여있다면) 뒤로 보냄
			return (MonsterA != nullptr) && (MonsterB == nullptr);
			});
	}
	break;

	case ETargetFilterType::HighestHP:
	{
		// 체력 내림차순 (높은 놈이 앞)
		Context.Targets.Sort([](const AActor& A, const AActor& B) {
			const auto* MonsterA = Cast<ADEMonsterBase>(&A);
			const auto* MonsterB = Cast<ADEMonsterBase>(&B);

			if (MonsterA && MonsterB)
			{
				return MonsterA->GetCurrentHP() > MonsterB->GetCurrentHP();
			}
			return (MonsterA != nullptr) && (MonsterB == nullptr);
			});
	}
	break;
	}

	// 3. 자르기 (Truncate)
	// 정렬이 끝났으니 앞에서부터 N개만 남기고 뒤는 다 날림
	Context.Targets.SetNum(TargetCount);
	for (const AActor* Target : Context.Targets)
	{
		UE_LOG(LogTemp, Warning, TEXT("Targets filtered : %s"), *Target->GetName());
	}
	// 배열 메모리 최적화 (선택 사항)
	// Context.Targets.Shrink();
}
