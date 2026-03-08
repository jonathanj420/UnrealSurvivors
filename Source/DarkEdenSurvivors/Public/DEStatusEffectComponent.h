// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DEStatusEffectTypes.h"
#include "DEStatusEffectBase.h"
#include "DEStatusEffectComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEffectChanged, class UDEStatusEffectBase*, Effect, bool, bAdded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEStatusEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDEStatusEffectComponent();

	// ─── 외부 인터페이스 ──────────────────────────────────

	// 상태이상 추가 함수 (CDO를 활용한 가비지 컬렉터 최적화 적용 완료)
	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	void AddEffect(TSubclassOf<UDEStatusEffectBase> EffectClass, AActor* Instigator, float Duration, float Power, float Interval = 0.f);

	// 특정 태그를 가진 모든 상태이상 제거 (예: 해독 물약, 정화 스킬)
	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	void RemoveEffectsByTag(EEffectTag Tag);

	// 모든 상태이상 제거 (예: 캐릭터 사망 시, 무적기 사용 시)
	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	void RemoveAllEffects();

	// ─── 조회 인터페이스 ──────────────────────────────────

	UFUNCTION(BlueprintPure, Category = "StatusEffect")
	bool HasEffectWithTag(EEffectTag Tag) const;

	UFUNCTION(BlueprintPure, Category = "StatusEffect")
	UDEStatusEffectBase* GetEffectByTag(EEffectTag Tag) const;

	// ─── 면역 시스템 ──────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	void AddImmunity(EEffectTag Tag) { ImmuneTags.AddUnique(Tag); }

	UFUNCTION(BlueprintCallable, Category = "StatusEffect")
	void RemoveImmunity(EEffectTag Tag) { ImmuneTags.Remove(Tag); }

	// ─── 이벤트 ───────────────────────────────────────────

	// HUD 위젯 등에서 이 이벤트를 바인딩해두면, 상태이상이 걸리거나 풀릴 때 아이콘을 띄울 수 있습니다.
	UPROPERTY(BlueprintAssignable, Category = "StatusEffect")
	FOnEffectChanged OnEffectChanged;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// 현재 활성화된 상태이상 목록
	UPROPERTY()
	TArray<UDEStatusEffectBase*> ActiveEffects;

	// 현재 면역인 태그 목록 (예: 보스 몬스터는 EEffectTag::Stun을 여기에 넣어두면 기절 안 걸림)
	UPROPERTY()
	TArray<EEffectTag> ImmuneTags;

	// 핵심 변경점: 몬스터뿐만 아니라 플레이어(DEFemaleVampire)도 상태이상에 걸릴 수 있도록 AActor로 범용화 캐싱
	UPROPERTY()
	TWeakObjectPtr<AActor> OwnerActor;
};
