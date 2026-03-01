// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DESkillData.generated.h"

//USTRUCT(BlueprintType)
//struct FSkillSpec
//{
//    GENERATED_BODY()
//
//public:
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    int32 Level = 1;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    int32 ProjectileCount = 1;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    float Damage = 5.f;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    int32 Penetration = 1;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    float Cooldown = 2.f;
//
//};

USTRUCT(BlueprintType)
struct FDESkillData : public FTableRowBase
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SkillID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Level;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SkillName;

    //stats
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Amount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Penetration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Cooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float KnockbackForce;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Speed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CritChance = 0.0f;

    // (선택) 치명타 배율도 스킬마다 다르게 하고 싶다면?
    // 0.0f면 플레이어 기본 배율(1.5) 따름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CritDamageMultiplier = 0.0f;
    // hitinterval, bleed... or smth
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, float> OptionValues;

};

USTRUCT(BlueprintType)
struct FDESkillRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 SkillID;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<class UDESkillBase> SkillClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText SkillName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* SkillIcon;

    UPROPERTY(EditAnywhere, Category = "Evolution")
    bool bIsEvolutionResult = false; // 진화 결과 스킬이면 true
};