// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DESimpleAOEBase.generated.h"

struct FDESkillContext;
class USphereComponent;
class UNiagaraComponent;

UCLASS()
class DARKEDENSURVIVORS_API ADESimpleAOEBase : public AActor
{
    GENERATED_BODY()

public:
    ADESimpleAOEBase();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

    void ApplyContext(const FDESkillContext& Context);

    // 풀에서 꺼낼 때 호출하는 초기화 함수
    void InitializeAOE(
        float InDamage,
        float InRadius,
        float InHitCooldown,
        float InLifeTime
    );
    // [추가] 컨텍스트 기반 초기화 (프로젝타일과 통일성 유지)
    virtual void InitializeFromContext(const FDESkillContext& Context);
    // 풀로 반환
    void ReturnToPool();

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

    // 데미지 적용
    void DealDamage(AActor* Target);

    virtual void LifeSpanExpired() override;

protected:
    TWeakObjectPtr<AActor> DamageInstigator;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UNiagaraComponent* NiagaraComp;

    // --- 스탯 변수 ---
    float Damage;
    float HitCooldown; // 개별 쿨타임 (Tick 간격)
    float LifeTime;
    bool bInfiniteDuration;

    TWeakObjectPtr<AActor> OwnerActor;

    // 개별 타격 쿨타임 기록 [Key: 적, Value: 다음 타격 가능 시간]
    TMap<AActor*, double> HitCooldownMap;
};