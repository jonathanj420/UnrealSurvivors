// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_SpawnSummon.generated.h"

class ADESimpleSummonBase;

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_SpawnSummon : public UDESkillBehavior
{
    GENERATED_BODY()

public:
    virtual void Execute(FDESkillContext& Context) override;

    // 소환할 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Summon")
    TSubclassOf<ADESimpleSummonBase> SummonClass;

    // 소환 시 재생할 사운드
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Summon")
    USoundBase* SpawnSound;

    // 시전자 기준 스폰 위치 오프셋 (예: 앞쪽으로 100만큼 띄워서 소환)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Summon")
    float SpawnDistanceOffset = 100.0f;

    bool bIgnoreAmount = true;

};
