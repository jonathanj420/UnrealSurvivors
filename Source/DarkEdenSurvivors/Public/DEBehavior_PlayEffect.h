// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_PlayEffect.generated.h"


class UNiagaraSystem;
class USoundBase;

// 이펙트를 어디서 재생할지 결정하는 Enum
UENUM(BlueprintType)
enum class EEffectTargetType : uint8
{
    Instigator   UMETA(DisplayName = "시전자 위치"),
    AllTargets   UMETA(DisplayName = "모든 타겟 위치"),
    TargetCenter UMETA(DisplayName = "타겟 중앙"),
    CustomLocations UMETA(DisplayName = "커스텀 위치")
};

UENUM(BlueprintType)
enum class EEffectRotation : uint8
{
    ZeroRotator,
    InstigatorForward,
    TowardTarget,
    RandomYaw
};

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_PlayEffect : public UDESkillBehavior
{
    GENERATED_BODY()

public:
    virtual void Execute(FDESkillContext& Context) override;

    UPROPERTY(EditAnywhere, Category = "Effect")
    UNiagaraSystem* NiagaraEffect = nullptr;

    UPROPERTY(EditAnywhere, Category = "Effect")
    USoundBase* SoundEffect = nullptr;

    UPROPERTY(EditAnywhere, Category = "Effect")
    EEffectTargetType TargetType = EEffectTargetType::AllTargets;

    UPROPERTY(EditAnywhere, Category = "Effect")
    EEffectRotation RotationType = EEffectRotation::ZeroRotator;

    UPROPERTY(EditAnywhere, Category = "Effect")
    FVector Offset = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Effect")
    bool bAttachToActor = false;

    UPROPERTY(EditAnywhere, Category = "Effect")
    bool bAutoDestroy = true;

    UPROPERTY(EditAnywhere, Category = "Effect|Size")
    FName SizeVariableName = NAME_None;

    UPROPERTY(EditAnywhere, Category = "Effect|Size")
    float SizeMultiplier = 1.0f;

public:
    // 나이아가라 이펙트가 터질 위치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    EEffectTargetType NiagaraTargetType;

    // 사운드가 재생될 위치 (분리!)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    EEffectTargetType SoundTargetType;

private:
    FRotator GetSpawnRotation(const FDESkillContext& Context, const FVector& SpawnLoc, AActor* Target) const;
    TArray<FVector> ResolveLocations(EEffectTargetType InTargetType, const FDESkillContext& Context) const;
};
