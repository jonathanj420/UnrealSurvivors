// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEActiveSkillBase.h"
#include "DESkill_BloodDrain.generated.h"


class ADEMonsterBase;
class ADECharacterBase;
class UNiagaraSystem;
class UDEStatusEffectBase;


/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_BloodDrain : public UDEActiveSkillBase
{
	GENERATED_BODY()
public:
    UDESkill_BloodDrain();
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

    /** 대상에게 거는 이동불가 상태이상. 몬스터 이동은 bCanMove만 보므로 상태이상을 거쳐야 멈추고, 죽으면 RemoveAllEffects로 풀린다 */
    UPROPERTY(EditDefaultsOnly, Category = "BloodDrain")
    TSubclassOf<UDEStatusEffectBase> StunEffectClass;

    //************* Effect and Sound**************
    /** 광흡 이펙트 (몬스터 → 플레이어) */
    UPROPERTY(EditDefaultsOnly, Category = "BloodDrain")
    UNiagaraSystem* BloodDrainEffect = nullptr;
    UPROPERTY(EditDefaultsOnly, Category = "BloodDrain")
    USoundBase* BloodDrainSound;

protected:
    // 몬스터와 잡을 당시의 SpawnSerial. 시전 중 다른 스킬에 죽고 풀에서 재사용된 몬스터는 마무리 대상에서 뺀다
    TArray<TPair<TWeakObjectPtr<ADEMonsterBase>, uint32>> CachedTargets;
    FTimerHandle BloodDrainTimerHandle;
private:
    /** 광흡 시작 처리 (스턴 + 이펙트) */
    void StartBloodDrain(ADECharacterBase* Player);

    /** 광흡 종료 처리 (데미지 + 힐) */
    void FinishBloodDrain();

    /** 주변 몬스터 수집 */
    void CollectTargets(TArray<ADEMonsterBase*>& OutTargets) const;

    void SpawnBloodDrainNiagara(
        ADECharacterBase* Player,
        const TArray<FVector>& MonsterPositions
    ) const;
    ///** 스턴 + 이펙트만 적용 */
    //void ApplyDrainEffect(
    //    ADEMonsterBase* Monster,
    //    ADECharacterBase* Player
    //) const;

    /** 종료 시 데미지 적용 */
    void ApplyFinishDamage(
        ADEMonsterBase* Monster,
        ADECharacterBase* Player
    ) const;
};
