// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_ApplyKnockback.h"
#include "DEMonsterBase.h"
#include "DESkillContext.h"

void UDEBehavior_ApplyKnockback::Execute(FDESkillContext& Context)
{
	// 1. 넉백 힘이 없으면 실행 안 함 (최적화)
	if (Context.KnockbackForce <= KINDA_SMALL_NUMBER) return;

	// 2. 기준점 설정: 무조건 "시전자(플레이어)" 위치
	// (투사체가 맞은 위치 HitLocation을 쓰지 않음!)
	FVector Origin = FVector::ZeroVector;
	if (Context.Instigator)
	{
		Origin = Context.Instigator->GetActorLocation();
	}

	// 3. 타겟 순회
	for (AActor* Target : Context.Targets)
	{
		if (!IsValid(Target)) continue;

		// 몬스터 클래스로 캐스팅 (ApplyKnockback 호출을 위해)
		ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Target);
		if (Monster)
		{
			// [핵심 로직] 방향 = 몬스터 위치 - 플레이어 위치
			// 결과: 플레이어로부터 멀어지는 벡터
			FVector Direction = Monster->GetActorLocation() - Origin;

			// Z축 무시 (수평 넉백)
			if (bFlattenZ)
			{
				Direction.Z = 0.0f;
			}

			// 예외 처리: 몬스터가 플레이어와 완벽하게 겹쳐서 벡터가 0인 경우
			if (Direction.IsNearlyZero())
			{
				// 몬스터가 바라보는 방향의 반대(뒷걸음질)로 밀어버림
				Direction = Monster->GetActorForwardVector() * -1.0f;
			}
			else
			{
				Direction.Normalize();
			}

			// 최종 힘 계산 (Context 값 * 배율)
			float FinalForce = Context.KnockbackForce * ForceMultiplier;

			// 몬스터 내부 로직 호출 (MoveToPlayer에서 처리됨)
			Monster->ApplyKnockback(Direction, FinalForce);
		}
	}
}