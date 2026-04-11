// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DESkillContext.h"
#include "DEStatTypes.h"
#include "DESimpleSummonBase.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UDECombatEffect;

UCLASS()
class DARKEDENSURVIVORS_API ADESimpleSummonBase : public AActor
{
	GENERATED_BODY()

public:
	ADESimpleSummonBase();
	virtual void Tick(float DeltaTime) override;

	// 풀링 및 초기화 (투사체와 동일)
	virtual void InitializeFromContext(const FDESkillContext& Context);
	virtual void ResetState();
	virtual void ReturnToPool();

protected:
	virtual void BeginPlay() override;

	// 수명 종료
	UFUNCTION()
	virtual void OnLifeTimeExpired();

	// ★ 갓-벽하게 짜두신 데미지 함수 그대로 사용
	bool TryDealDamage(AActor* Victim);

	// --- 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* NiagaraComponent;

	// --- 스킬 데이터 (투사체와 동일) ---
	UPROPERTY()
	FDESkillContext CachedContext;

	FCombatSnapshot Snapshot;

	float Damage = 0.0f;
	float CritChance = 0.0f;
	float CritDamageMultiplier = 1.0f;
	float LifeTime = 5.0f;
	float EffectRadius = 0.0f;
	float KnockbackForce = 0.0f;

	// 로컬 이펙트 (처형, 피흡 등)
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Effects")
	TArray<UDECombatEffect*> LocalEffects;

	FTimerHandle LifeTimeTimerHandle;


};
