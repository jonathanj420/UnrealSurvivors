// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DEMonsterDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEMonsterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

    UPROPERTY(EditDefaultsOnly, Category = "Monster")
    int32 MonsterID = 0;

    // 내부 식별용 이름 (디버그 표기용)
    UPROPERTY(EditDefaultsOnly, Category = "Monster")
    FName InternalName;

    // UI 표기용 이름
    UPROPERTY(EditDefaultsOnly, Category = "Monster")
    FText DisplayName;
    // 외형 (SkeletalMesh 또는 StaticMesh 중 하나 선택)
    UPROPERTY(EditAnywhere, Category = "Appearance")
    USkeletalMesh* Mesh = nullptr;

    UPROPERTY(EditAnywhere, Category = "Appearance")
    UAnimationAsset* WalkAnimation = nullptr;

    // 기본 스탯
    UPROPERTY(EditAnywhere, Category = "Stats")
    float MaxHP = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float MoveSpeed = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float Damage = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float KnockbackResistance = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float CollisionRadius = 30.0f;

    UPROPERTY(EditAnywhere, Category = "Misc")
    int32 EXPDrop = 1;

    UPROPERTY(EditAnywhere, Category = "Misc")
    float Scale = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Misc")
    FLinearColor TintColor = FLinearColor::White;

};
