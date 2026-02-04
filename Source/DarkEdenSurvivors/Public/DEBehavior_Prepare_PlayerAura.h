// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_Prepare_PlayerAura.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_Prepare_PlayerAura : public UDESkillBehavior
{
    GENERATED_BODY()

public:
    virtual void Execute(FDESkillContext& Context) override;

public:
    // 실제 갈릭 오라 AOE 클래스
    UPROPERTY(EditDefaultsOnly, Category = "AOE")
    TSubclassOf<class ADESimpleAOEBase> AuraAOEClass;

    // Persistent AOE 식별자 (갈릭은 항상 동일)
    UPROPERTY(EditDefaultsOnly, Category = "AOE")
    FName AuraAOEKey = TEXT("PlayerAura");
	
};
