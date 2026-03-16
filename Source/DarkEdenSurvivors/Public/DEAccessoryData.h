// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "DEEffectTypes.h"
#include "DEStatTypes.h"
#include "DEAccessoryData.generated.h"


class UDECombatEffect;
/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEAccessoryData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    int32 ID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    UTexture2D* Icon = nullptr;


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    int32 MaxLevel = 5;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    TArray<FDEStatModifier> StatModifiers;

   /* UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    TArray<FEffectData> Effects;*/

    //  구버전: TArray<FEffectData> Effects; (삭제!)
    //  신버전: 어떤 이펙트(클래스)들을 가지고 있는지 클래스 정보만 저장!
    UPROPERTY(EditAnywhere, Instanced, Category = "Effects")
    TArray<UDECombatEffect*> AccessoryEffects;

};

USTRUCT(BlueprintType)
struct FDEAccessoryRow : public FTableRowBase
{
    GENERATED_BODY()

    // 어떤 악세서리인가? (위에서 만든 데이터 에셋을 쏙 넣음)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UDEAccessoryData* AccessoryData = nullptr;

    // 등장 확률 가중치 (기본 100. 전설템은 10~20으로 낮추면 됨!)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DropWeight = 100.0f;
};
