// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESimpleProjectileBase.h"
#include "DEProjectile_SanguineBat.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEProjectile_SanguineBat : public ADESimpleProjectileBase
{
	GENERATED_BODY()
	
public:
	ADEProjectile_SanguineBat();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* SkeletalMesh;

public:
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;
};
