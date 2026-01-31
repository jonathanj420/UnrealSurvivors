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
    int32 ProjectileCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Penetration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Cooldown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Radius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float KnockbackForce;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ProjectileSpeed;
    // stun, bleed... or smth
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
};