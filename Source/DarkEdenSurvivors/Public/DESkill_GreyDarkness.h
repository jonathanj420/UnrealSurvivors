// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEAutoSkillBase.h"
#include "DESkill_GreyDarkness.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkill_GreyDarkness : public UDEAutoSkillBase
{
	GENERATED_BODY()
	
public:
	UDESkill_GreyDarkness();

	virtual void InitBehaviors() override;
protected:
    float AccumulatedDamage = 0.0f;
    float AccumulatedRadius = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Growth") float BonusDamagePerKill = 0.5f;
    UPROPERTY(EditAnywhere, Category = "Growth") float BonusRadiusPerKill = 2.0f;
    UPROPERTY(EditAnywhere, Category = "Growth") float MaxBonusDamage = 30.0f;
    UPROPERTY(EditAnywhere, Category = "Growth") float MaxBonusRadius = 150.0f;

public:
    // ★ 인터페이스 함수 구현
    virtual void OnTargetKilled(const FDEDamageResult& Result) override;

protected:
    // 스택을 최종 스킬 컨텍스트에 밀어넣기
    virtual void BuildContext(FDESkillContext& OutContext) override;

private:

	TSubclassOf<AActor> AOEClass;

};
