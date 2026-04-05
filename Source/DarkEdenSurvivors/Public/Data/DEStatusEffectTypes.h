// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEStatusEffectTypes.generated.h"
class UDEStatusEffectBase;

USTRUCT(BlueprintType)
struct FActiveStatusEffect
{
	GENERATED_BODY()

	// 1. 거푸집(CDO) 포인터: 변하지 않는 로직과 세팅값에 접근하기 위함
	UPROPERTY()
	const UDEStatusEffectBase* EffectDef = nullptr;

	// 2. 변하는 상태값들 (개발자님의 완벽한 크래시 방어 철학을 이어받아 WeakPtr 사용)
	UPROPERTY()
	TWeakObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY()
	float ElapsedTime = 0.f;

	UPROPERTY()
	float TickTimer = 0.f;

	UPROPERTY()
	float Duration = 0.f;

	UPROPERTY()
	float Power = 0.f;

	UPROPERTY()
	float Interval = 0.f;

	UPROPERTY()
	int32 CurrentStacks = 1;

	// 구조체 초기화를 위한 편의 생성자
	FActiveStatusEffect() {}
};


UENUM(BlueprintType)
enum class EStackPolicy : uint8
{
    Replace   UMETA(DisplayName = "Replace"),
    Refresh   UMETA(DisplayName = "Refresh"),
    Stack     UMETA(DisplayName = "Can Stack"),
    Ignore    UMETA(DisplayName = "Ignore")
};