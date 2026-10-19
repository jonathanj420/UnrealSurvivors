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

public:

    // [추가] 컨텍스트 기반 초기화 (프로젝타일과 통일성 유지)
    virtual void InitializeFromContext(const FDESkillContext& Context);
    // 풀로 반환
    virtual void ResetState() override;
    virtual void ReturnToPool() override;

protected:
    // 몬스터 캡슐은 GenerateOverlapEvents가 꺼져 있어서 오버랩 이벤트 대신 씬 쿼리로 찾는다
    void QueryTargets(TArray<AActor*>& OutTargets) const;

    // 타격 가능 여부 확인 (개별 쿨타임 체크, bHitOnContact일 때만 사용)
    virtual bool CanHitTarget(AActor* Target) const override;

    // 실제 타격 처리 (가상함수: 자식에서 오버라이드 가능)
    virtual void OnHitTarget(AActor* Target);


protected:

    float HitCooldown=0.5f; // 컨텍스트의 HitInterval

    // false: HitInterval마다 범위 안의 적을 한꺼번에 타격 (장판)
    // true: 매 Tick 쿼리하고 타겟별로 HitInterval 쿨다운 (움직이는 Orbit처럼 닿는 순간 맞아야 하는 경우)
    bool bHitOnContact = false;

    // 다음 펄스까지 남은 시간. 0이면 활성화 직후 첫 Tick에 바로 타격
    float PulseTimer = 0.0f;

    bool bInfiniteDuration;
};