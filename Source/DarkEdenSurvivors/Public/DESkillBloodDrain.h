// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEActiveSkillBase.h"
#include "DESkillBloodDrain.generated.h"


class ADEMonsterBase;
class ADECharacterBase;
class UNiagaraSystem;


/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillBloodDrain : public UDEActiveSkillBase
{
	GENERATED_BODY()
public:
    UDESkillBloodDrain();
    virtual void ActivateSkill() override;
        
protected:
    //Drain Status
    /** 광흡 범위 */
    UPROPERTY(EditDefaultsOnly, Category = "BloodDrain")
    float DrainRadius = 500.f;

    /** 몬스터 1마리당 흡혈량 */
    UPROPERTY(EditDefaultsOnly, Category = "BloodDrain")
    float DrainAmountPerMonster = 5.f;

    /** 몬스터 스턴 시간 */
    UPROPERTY(EditDefaultsOnly, Category = "BloodDrain")
    float StunDuration = 3.0f;

    //************* Effect and Sound**************
    /** 광흡 이펙트 (몬스터 → 플레이어) */
    UPROPERTY(EditDefaultsOnly, Category = "BloodDrain")
    UNiagaraSystem* BloodDrainEffect = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "BloodDrain")
    USoundBase* BloodDrainSound;

protected:
    TArray<TWeakObjectPtr<ADEMonsterBase>> CachedTargets;
    FTimerHandle BloodDrainTimerHandle;
private:
    /** 광흡 시작 처리 (스턴 + 이펙트) */
    void StartBloodDrain(ADECharacterBase* Player);

    /** 광흡 종료 처리 (데미지 + 힐) */
    void FinishBloodDrain();

    /** 주변 몬스터 수집 */
    void CollectTargets(TArray<ADEMonsterBase*>& OutTargets) const;

    /** 스턴 + 이펙트만 적용 */
    void ApplyDrainEffect(
        ADEMonsterBase* Monster,
        ADECharacterBase* Player
    ) const;

    /** 종료 시 데미지 적용 */
    void ApplyFinishDamage(
        ADEMonsterBase* Monster,
        ADECharacterBase* Player
    ) const;
};
