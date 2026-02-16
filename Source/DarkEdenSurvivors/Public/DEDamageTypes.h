// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEDamageTypes.generated.h"

/**
 * 
 */

 // [요청] 때리는 놈이 보내는 정보
USTRUCT(BlueprintType)
struct FDEDamageRequest
{
	GENERATED_BODY()

	// 누가 때렸나? (플레이어)
	UPROPERTY(BlueprintReadWrite)
	AActor* Instigator = nullptr;

	// 무엇으로 때렸나? (투사체, 칼 등)
	UPROPERTY(BlueprintReadWrite)
	AActor* DamageCauser = nullptr;

	// [추가] 물리적인 액터는 아니지만, 데미지의 원인이 된 스킬/아이템 객체
	// (이게 있어야 킬로그에서 스킬 아이콘을 보여줄 수 있음)
	UPROPERTY(BlueprintReadWrite)
	UObject* SourceObject = nullptr;

	// 기본 데미지 (아직 치명타/방어력 적용 전)
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.0f;

	// 치명타 확률 (0.0 ~ 1.0)
	UPROPERTY(BlueprintReadWrite)
	float CritChance = 0.0f;

	// 치명타 배율 (예: 5.8 = 580%)
	UPROPERTY(BlueprintReadWrite)
	float CritDamageMultiplier = 1.0f;

	// (확장성) 방어 무시 여부, 속성 타입 등은 나중에 여기에 추가
};

// [결과] 맞은 놈이 계산해서 돌려주는 정보
USTRUCT(BlueprintType)
struct FDEDamageResult
{
	GENERATED_BODY()

		// 최종 적용된 데미지
		UPROPERTY(BlueprintReadWrite)
	float FinalDamage = 0.0f;

	// 치명타 터졌는지? (UI 출력용)
	UPROPERTY(BlueprintReadWrite)
	bool bIsCritical = false;

	// 그래서 죽었는지? (킬 로그, 흡혈용)
	UPROPERTY(BlueprintReadWrite)
	bool bIsDead = false;

	// (확장성) 회피했는지? 막았는지?
	// bool bIsEvaded = false;
};

USTRUCT(BlueprintType)
struct FDamageVisualInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    float Amount = 0.f;

    UPROPERTY(BlueprintReadOnly)
    FVector WorldLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    bool bIsCritical = false;
};//zis for floating damage num