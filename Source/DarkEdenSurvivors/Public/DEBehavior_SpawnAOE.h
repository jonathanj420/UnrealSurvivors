// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_SpawnAOE.generated.h"

UENUM(BlueprintType)
enum class EAOESpawnTarget : uint8
{
    Instigator,        // 내 몸뚱이
    AllTargets,        // 필터링된 타겟 몬스터들 전원 (각각 1개씩)
    CustomLocations    // 특정 바닥 좌표 (메테오, 눈보라 등)
};


/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_SpawnAOE : public UDESkillBehavior
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "AOE")
    TSubclassOf<class ADESimpleAOEBase> AOEClass;

    // ★ 핵심: 장판을 누구한테/어디에 깔지 결정!
    UPROPERTY(EditAnywhere, Category = "AOE")
    EAOESpawnTarget SpawnTarget = EAOESpawnTarget::Instigator;

    // ★ 핵심: 스폰된 위치의 주인(Actor)에게 찰싹 달라붙을 것인가?
    UPROPERTY(EditAnywhere, Category = "AOE")
    bool bAttachToTarget = false;

    UPROPERTY(EditAnywhere, Category = "AOE")
    FName AOEKey = NAME_None;

    virtual void Execute(FDESkillContext& Context) override;
};
