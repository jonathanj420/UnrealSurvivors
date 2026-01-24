// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "NiagaraSystem.h"
#include "DEBehavior_PlayNiagara.generated.h"

UENUM(BlueprintType)
enum class EEffectSpawnLocation : uint8
{
	Instigator,      // 시전자 위치
	TargetActors,    // 타겟들 위치
	CustomLocations  // 특정 좌표들
};

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_PlayNiagara : public UDESkillBehavior
{

	GENERATED_BODY()

public:
	virtual void Execute(FDESkillContext& Context) override;

public:
	// 재생할 에셋
	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<UNiagaraSystem> NiagaraAsset;

	// 위치 기준 (위에 선언한 Enum 바로 사용)
	UPROPERTY(EditAnywhere, Category = "VFX")
	EEffectSpawnLocation SpawnLocation = EEffectSpawnLocation::TargetActors;

	// 위치 오프셋
	UPROPERTY(EditAnywhere, Category = "VFX")
	FVector Offset = FVector::ZeroVector;

	// 위치 오프셋
	UPROPERTY(EditAnywhere, Category = "VFX")
	float SizeMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "VFX")
	FName SizeVariableName;

	// 부착 여부
	UPROPERTY(EditAnywhere, Category = "VFX")
	bool bAttachToActor = false;

	
};
