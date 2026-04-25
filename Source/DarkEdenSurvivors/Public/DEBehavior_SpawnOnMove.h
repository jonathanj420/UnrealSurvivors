// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DESkillContext.h"
#include "Engine/EngineTypes.h"
#include "DEBehavior_SpawnOnMove.generated.h"

class ADESkillActorBase;
class UDEAutoSkillBase;

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_SpawnOnMove : public UDESkillBehavior
{
    GENERATED_BODY()

public:
    virtual void Execute(FDESkillContext& Context) override;

    // 영구 스킬 스탯 갱신 시 호출: 이미 깔린 액터들의 스탯 업데이트 및 죽은 액터 청소
    virtual void OnContextRefreshed(const FDESkillContext& Context) override;

    // 스킬 진화/해제 시 타이머를 안전하게 끄기 위한 가상 함수 오버라이드
    virtual void EndBehavior() override;

protected:
    // ★ 장판(AOE)에 국한되지 않는 완벽한 다형성! 투사체, 소환수 뭐든 다 됨
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Settings")
    TSubclassOf<ADESkillActorBase> SpawnActorClass;

    // 이 거리 이상 이동하면 생성 (기본 50cm)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Settings")
    float SpawnDistanceThreshold = 50.0f;

    // 거리 검사 주기 (0.1 ~ 0.2초 권장)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Settings")
    float CheckInterval = 0.2f;

private:
    void CheckAndSpawn();

    FVector LastSpawnLocation;
    FTimerHandle DistanceCheckTimerHandle;

    // 타이머 콜백 내부 크래시 방지용 안전한 캐싱
    TWeakObjectPtr<AActor> CachedInstigator;
    TWeakObjectPtr<UDEAutoSkillBase> CachedSourceSkill;
	
};
