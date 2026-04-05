// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DEStatusEffectTypes.h"
#include "DEStatusEffectBase.h"
#include "GameplayTagContainer.h"
#include "DEStatusEffectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDEStatusEffectComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Status Effect")
	void AddEffect(TSubclassOf<UDEStatusEffectBase> EffectClass, AActor* Instigator, float Duration, float Power, float Interval);

	// =========================================================
	// ★ [수정됨] EEffectTag -> FGameplayTag 로 모두 교체!
	// =========================================================
	UFUNCTION(BlueprintCallable, Category = "Status Effect")
	void RemoveEffectsByTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = "Status Effect")
	void RemoveAllEffects();

	UFUNCTION(BlueprintPure, Category = "Status Effect")
	bool HasEffectWithTag(FGameplayTag Tag) const;

	void ProcessIncomingDamageModifiers(struct FDEDamageRequest& InOutRequest);

protected:
	TWeakObjectPtr<AActor> OwnerActor;

	// =========================================================
	// ★ [수정됨] TSet<EEffectTag> -> FGameplayTagContainer 가방으로 교체!
	// 이제 보스 몬스터 블루프린트에서 "Status.Debuff.Stun" (기절) 태그를 추가해두면 
	// 알아서 기절 면역이 됩니다.
	// =========================================================
	UPROPERTY(EditDefaultsOnly, Category = "Status Effect", meta = (Categories = "Status"))
	FGameplayTagContainer ImmuneTags;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status Effect")
	TArray<FActiveStatusEffect> ActiveEffects;

	// ★ [수정됨] 내부 검색 헬퍼 함수도 FGameplayTag 로 교체
	FActiveStatusEffect* GetEffectByTag(FGameplayTag Tag);
};
