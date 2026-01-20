// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEStatusEffectBase.h"
#include "DEStatusEffectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDEStatusEffectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // 상태이상 추가 함수 (가장 중요!)
// TSubclassOf를 받아서 내부적으로 NewObject 함
    void AddEffect(TSubclassOf<UDEStatusEffectBase> EffectClass, AActor* Instigator, float Duration, float Power, float Interval = 0.f);

    // 모든 상태이상 제거 (죽을 때 호출)
    void RemoveAllEffects();

protected:
    UPROPERTY()
    TArray<UDEStatusEffectBase*> ActiveEffects;

    // 타겟(주인) 캐싱
    class ADEMonsterBase* OwnerMonster;
};
