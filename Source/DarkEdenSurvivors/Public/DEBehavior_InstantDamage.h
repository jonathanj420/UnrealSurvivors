// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DESkillContext.h"
#include "GameplayTagContainer.h"
#include "DEBehavior_InstantDamage.generated.h"

/**
 *
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_InstantDamage : public UDESkillBehavior
{
	GENERATED_BODY()
public:
	virtual void Execute(FDESkillContext& Context) override;
	virtual bool IsInProgress() const override;
	virtual void EndBehavior() override;

	// ★ [NEW] 이 레고 블록이 터뜨릴 데미지의 속성과 판정 태그들!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (Categories = "Damage"))
	FGameplayTagContainer SkillDamageTags;

	// 데미지 배율 (첫 타격 2배! 같은 거 할 때 씀)
	UPROPERTY(EditAnywhere)
	float DamageMultiplier = 1.0f;

	// 같은 타겟들을 몇 번 때릴지. 첫 타격은 Execute 때 바로 들어간다
	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	int32 HitCount = 1;

	// 타격 사이 간격(초). 0이면 HitCount만큼 한 번에 때린다.
	// 0보다 크면 남은 타격은 타이머로 들어가고, 파이프라인의 다음 Behavior는 기다리지 않고 바로 실행된다
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	float HitInterval = 0.0f;

private:
	// Targets 전원에게 한 번 타격. 죽은 타겟은 Targets에서 뺀다
	void ApplyHit(const FDESkillContext& Context, TArray<AActor*>& Targets);
	void ApplyNextHit();

	// 간격을 둔 타격용. 타이머가 나중에 실행되므로 Context를 복사해 둔다
	UPROPERTY()
	FDESkillContext PendingContext;

	int32 RemainingHits = 0;
	FTimerHandle HitTimerHandle;
};
