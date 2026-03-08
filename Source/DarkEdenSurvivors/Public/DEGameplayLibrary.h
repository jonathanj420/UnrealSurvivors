// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DEDamageTypes.h" 
#include "DEStatTypes.h"
#include "DEGameplayLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEGameplayLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FDEDamageResult ApplyCombatDamage(
		const FDEDamageRequest& Request, // 이미 완성된 영수증
		const FCombatSnapshot& Snapshot, // 후처리(피흡 등)를 위한 스탯 뭉치
		FVector KnockbackDir = FVector::ZeroVector,
		float KnockbackForce = 0.0f
	);
	static AActor* GetNearestTarget(AActor* Instigator, float Radius);
	static TArray<AActor*> GetRandomTargets(AActor* Instigator, float Radius, int32 Count);
	
};
