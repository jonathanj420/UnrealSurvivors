// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DECombatEffect.h"
#include "DESkillTypes.h"
#include "DECombatEffect_CooldownReduction.generated.h"

UENUM(BlueprintType)
enum class ECDRTargetSkill : uint8
{
	AllSkills      UMETA(DisplayName = "모든 스킬"),
	SpecificSkill  UMETA(DisplayName = "특정 스킬 지정")
};

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDECombatEffect_CooldownReduction : public UDECombatEffect
{
    GENERATED_BODY()

public:
    // 고정값인지 퍼센트인지
    UPROPERTY(EditAnywhere, Category = "Cooldown")
    ECooldownReduceType ReduceType = ECooldownReduceType::PercentageOfMax;

    // 깎을 수치
    UPROPERTY(EditAnywhere, Category = "Cooldown")
    float Amount = 0.1f;

    // 대상을 어떻게 잡을 것인가? 드롭다운 메뉴 생성!
    UPROPERTY(EditAnywhere, Category = "Cooldown")
    ECDRTargetSkill TargetSkill = ECDRTargetSkill::SpecificSkill;

    // ★ 미친 꿀팁: EditCondition
    // TargetSkill 드롭다운에서 'SpecificSkill'을 선택했을 때만 이 입력창이 활성화됨!
    UPROPERTY(EditAnywhere, Category = "Cooldown", meta = (EditCondition = "TargetSkill == ECDRTargetSkill::SpecificSkill", EditConditionHides))
    int32 SpecificSkillID = -1;

    // 처치 시 쿨초기화 뽕맛용
    UPROPERTY(EditAnywhere, Category = "Cooldown")
    bool bInstantReset = false;

    virtual void OnExecuteEffect(FCombatEventData& EventData) override;
	
};
