// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESimpleSummonBase.h"
#include "DESummon_GoreGland.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADESummon_GoreGland : public ADESimpleSummonBase
{
    GENERATED_BODY()

public:
    ADESummon_GoreGland();
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void ResetState() override;
    virtual void ReturnToPool() override;

    // --- 고어 글랜드 전용 AI 로직 ---
    UFUNCTION()
    void FindNearestTarget();

    UFUNCTION()
    void PerformBloodPulse();

    // --- 기획 파라미터 ---
    UPROPERTY(EditDefaultsOnly, Category = "GoreGland|Combat")
    float AttackInterval = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "GoreGland|Combat")
    float AttackRadius = 300.0f;

    UPROPERTY(EditDefaultsOnly, Category = "GoreGland|Movement")
    float MoveSpeed = 250.0f;

    UPROPERTY(EditDefaultsOnly, Category = "GoreGland|Movement")
    float TargetSearchRadius = 1500.0f;

public:
    // 에디터에서 폭발 나이아가라 에셋을 넣을 슬롯
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GoreGland|Effect")
    class UNiagaraSystem* BloodPoolFX;

private:
    FTimerHandle TargetSearchTimerHandle;
    FTimerHandle AttackTimerHandle;

    // 현재 쫓고 있는 사냥감
    UPROPERTY()
    AActor* CurrentTarget;
	
};
