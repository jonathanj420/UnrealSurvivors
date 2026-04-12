// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillActorBase.h"
#include "DESimpleAOEBase.generated.h"


UCLASS()
class DARKEDENSURVIVORS_API ADESimpleAOEBase : public ADESkillActorBase
{
    GENERATED_BODY()

public:
    ADESimpleAOEBase();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

    // [추가] 컨텍스트 기반 초기화 (프로젝타일과 통일성 유지)
    virtual void InitializeFromContext(const FDESkillContext& Context);
    // 풀로 반환
    virtual void ResetState() override;
    virtual void ReturnToPool() override;

protected:
    // 들어오자마자 즉시 반응하기 위한 이벤트 (반응성 핵심)
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    // 타격 가능 여부 확인 (개별 쿨타임 체크)
    bool CanHitTarget(AActor* Target) const;

    // 실제 타격 처리 (가상함수: 자식에서 오버라이드 가능)
    virtual void OnHitTarget(AActor* Target);


protected:

    float HitCooldown=0.5f; // 개별 쿨타임 (Tick 간격)

    bool bInfiniteDuration;

    // 개별 타격 쿨타임 기록 [Key: 적, Value: 다음 타격 가능 시간]
    TMap<AActor*, double> HitCooldownMap;
    // [추가] 타겟별 피격 횟수 기록 (관통력 제한 구현용)
    // Key: 맞은 놈, Value: 몇 대 맞았는지
    UPROPERTY()
    TMap<AActor*, int32> HitCountMap;
};