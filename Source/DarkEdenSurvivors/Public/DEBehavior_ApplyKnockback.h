// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_ApplyKnockback.generated.h"

/**
 * [넉백 비헤이비어]
 * - Context.Targets에 있는 적들을 "무조건 시전자(플레이어) 반대 방향"으로 밀어냅니다.
 * - 물리적 위치(투사체 충돌 지점)를 무시하고, 게임적 허용(Gameplay Feel)을 우선합니다.
 */
/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_ApplyKnockback : public UDESkillBehavior
{
	GENERATED_BODY()
public:
	virtual void Execute(FDESkillContext& Context) override;

public:
	// 넉백 힘 배율 (기본 1.0)
	// 예: 기본 수치는 1000인데, 이 스킬만 1.5배로 더 세게 밀고 싶을 때 사용
	UPROPERTY(EditAnywhere, Category = "Knockback")
	float ForceMultiplier = 1.0f;

	// Z축(위아래) 힘을 무시하고 수평으로만 밀지 여부 (기본 true)
	// true: 몬스터가 하늘로 뜨지 않고 바닥에서 미끄러짐
	UPROPERTY(EditAnywhere, Category = "Knockback")
	bool bFlattenZ = true;
};
