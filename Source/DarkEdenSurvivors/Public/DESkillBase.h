// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/DESkillData.h"
#include "DESkillBase.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillBase : public UObject
{
    GENERATED_BODY()

public:
    UDESkillBase();
    //virtual void ActivateSkill(FDESkillData* SkillData) PURE_VIRTUAL(UDESkillBase::ActivateSkill, );
    //FSkillSpec CurrentSpec;

    //FDESkillData* RowData = nullptr;  // 초기 스킬 데이터

    UPROPERTY()
    AActor* SkillOwner = nullptr;

    void SetOwner(AActor* InOwner) { SkillOwner = InOwner; }

    //// 최종 스펙 계산 (캐릭터 보정, 강화 옵션 적용 등)
    //virtual FSkillSpec GetFinalSpec(int32 CurrentLevel) const;
   

};
