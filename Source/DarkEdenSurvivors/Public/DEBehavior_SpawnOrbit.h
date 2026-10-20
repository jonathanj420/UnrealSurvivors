// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_SpawnOrbit.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_SpawnOrbit : public UDESkillBehavior
{
	/**
 * 성서형(Orbit) AOE를 여러 개 스폰하는 전용 Behavior
 * - Prepare 단계 없음
 * - AOERequests 사용 안 함
 * - Skill에서 OrbitAOEClass 직접 주입
 */
	GENERATED_BODY()

public:
	virtual void Execute(FDESkillContext& Context) override;
	virtual void OnContextRefreshed(const FDESkillContext& Context) override;

private:
	// 살아 있는 궤도를 Context.Amount개로 맞춘다. 있는 건 재사용하고 갱신, 모자라면 스폰, 남으면 반환
	void ArrangeOrbits(const FDESkillContext& Context, bool bSpawnIfNone);
	void SpawnOneOrbit(const FDESkillContext& Context, class UDEPoolSubsystem& Pool, float StartAngle) const;

public:
	/** 스폰할 Orbit AOE 클래스 (Danse Macabre, Skull 등) */
	UPROPERTY(EditDefaultsOnly, Category = "Orbit")
	TSubclassOf<class ADEAOE_OrbitBase> OrbitAOEClass;
};
