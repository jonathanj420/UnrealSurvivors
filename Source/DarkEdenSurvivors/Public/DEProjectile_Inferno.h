// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESimpleProjectileBase.h"
#include "DEProjectile_Inferno.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEProjectile_Inferno : public ADESimpleProjectileBase
{
	GENERATED_BODY()

public:
	ADEProjectile_Inferno();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	virtual void InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction) override;
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;
};
