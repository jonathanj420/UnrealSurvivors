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
		const FDEDamageRequest& Request // 이미 완성된 영수증
	);
	static void ApplyAoEDamage(
		UWorld* World,
		const FVector& Origin,
		float Radius,
		const FDEDamageRequest& BaseRequest, // 누구의 명의로, 데미지 얼마를 쏠 것인가?
		const TArray<AActor*>& IgnoredActors // 폭발에 맞지 않을 예외 대상들 (예: 본인)
	);

	//static void ApplyKnockback(AActor* TargetActor, FVector KnockbackDir, float KnockbackForce);
	static AActor* GetNearestTarget(AActor* Instigator, float Radius);
	static TArray<AActor*> GetRandomTargets(AActor* Instigator, float Radius, int32 Count);
	
};
