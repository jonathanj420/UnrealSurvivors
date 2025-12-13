// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBase.h"
#include "Data/DESkillData.h"
#include "DEAutoSkillBase.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEAutoSkillBase : public UDESkillBase
{
	GENERATED_BODY()
	
public:
    UDEAutoSkillBase();
    virtual void ActivateSkill(FDESkillData* SkillData) PURE_VIRTUAL(UDESkillBase::ActivateSkill, );
    FSkillSpec CurrentSpec;

    FDESkillData* RowData = nullptr;  // 초기 스킬 데이터

    // 최종 스펙 계산 (캐릭터 보정, 강화 옵션 적용 등)
    virtual FSkillSpec GetFinalSpec(int32 CurrentLevel) const;
};
