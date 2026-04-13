// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEStatusEffect_DelayedAction.h"
#include "DEStatusEffect_DelayedExplosion.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEStatusEffect_DelayedExplosion : public UDEStatusEffect_DelayedAction
{
    GENERATED_BODY()

protected:
    // 기획 데이터 세팅
    UPROPERTY(EditDefaultsOnly, Category = "Explosion")
    float ExplosionRadius = 300.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Explosion")
    class UNiagaraSystem* ExplosionParticle;

    // 부모가 시킨 숙제(폭발 로직) 구현
    virtual void ExecuteAction(AActor* Target, FActiveStatusEffect& EffectData) const override;
	
};
