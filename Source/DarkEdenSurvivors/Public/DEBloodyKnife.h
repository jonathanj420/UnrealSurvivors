// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESimpleProjectileBase.h"
#include "DEBloodyKnife.generated.h"


class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class DARKEDENSURVIVORS_API ADEBloodyKnife : public ADESimpleProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADEBloodyKnife();

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
    UPROPERTY(Transient)
    TArray<AActor*> HitActors;
   
public:
	virtual void InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction) override;
    void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult) override;
 


};
