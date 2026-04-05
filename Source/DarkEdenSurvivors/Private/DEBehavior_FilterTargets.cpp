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
	if (Context.Targets.Num() == 0) return;

	const int32 FinalCount = (TargetCount == -1) ? Context.Amount : TargetCount;

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
	/*Context.Targets.SetNum(FinalCount);
	for (const AActor* Target : Context.Targets)
	{
		UE_LOG(LogTemp, Warning, TEXT("Targets filtered : %s"), *Target->GetName());
	}*/

	if (Context.Targets.Num() > FinalCount)
	{
		// 케이스 A: 몹이 200마리로 너무 많다? 앞에서부터 6마리만 남기고 가차 없이 자르기!
		Context.Targets.SetNum(FinalCount);
	}
	else if (Context.Targets.Num() < FinalCount)
	{
		// 케이스 B: 몹은 1마리뿐인데 창은 6발이다?!
		// 있는 놈들을 '돌려막기(Round-Robin)'로 배열에 꽉꽉 채워 넣습니다!
		int32 OriginalNum = Context.Targets.Num();
		int32 Needed = FinalCount - OriginalNum; // 부족한 타겟 수 (6 - 1 = 5)

		for (int32 i = 0; i < Needed; ++i)
		{
			//네, 맞습니다.언리얼 TArray는 자기 자신의 원소를 직접 참조해서 곧바로 Add()에 집어넣는 행위 자체를 원천 차단(Assertion) 해두었습니다.메모리 오염(Memory Corruption)을 막기 위한 에픽게임즈의 안전장치입니다.
			// =========================================================
			// ★ 에러 해결: 배열의 원소를 지역 변수(포인터)로 안전하게 빼옵니다.
			// =========================================================
			AActor* TargetToCopy = Context.Targets[i % OriginalNum];

			// 그 복사해 둔 포인터를 안전하게 집어넣습니다! (크래시 절대 안 남)
			Context.Targets.Add(TargetToCopy);

			//// % (나머지 연산)을 쓰면 기가 막힌 분배가 일어납니다.
			//// 몹이 A 한 마리면? [A] -> [A, A, A, A, A, A]
			//// 몹이 A, B 두 마리면? [A, B] -> [A, B, A, B, A, B] (창 3발씩 공평하게 나눠 맞음!)
			//Context.Targets.Add(Context.Targets[i % OriginalNum]);
		}
	}

	// 배열 메모리 최적화 (선택 사항)
	// Context.Targets.Shrink();
}
