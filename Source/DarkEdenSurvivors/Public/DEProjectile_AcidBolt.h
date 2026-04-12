// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESimpleProjectileBase.h"
#include "DEProjectile_AcidBolt.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEProjectile_AcidBolt : public ADESimpleProjectileBase
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ADEProjectile_AcidBolt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// BloodyKnife를 초기화하는 함수 (스킬이 스폰할 때 호출)
	void Initialize(const FVector Direction);

protected:
	UPROPERTY(EditAnywhere, Category = "Damage")
	float ExplosionDamage = 60.f;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float ExplosionRadius = 180.f;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ExplosionSound;

public:
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;
	void Explode();
	virtual void OnLifeTimeExpired() override;

public:
	// 에디터에서 폭발 나이아가라 에셋을 넣을 슬롯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* ExplosionEffect;

};
