// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DELevelUpChoiceBase.h"
#include "Data/DESkillData.h"
#include "DELevelUpChoice_Skill.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDELevelUpChoice_Skill : public UDELevelUpChoiceBase
{
    GENERATED_BODY()

public:
    void Init(int32 InSkillID,
        FText InName, 
        UTexture2D* InIcon, 
        FText InDescription, 
        int32 InTargetLevel=0
    );

    // ===== UDELevelUpChoice =====
    virtual FText GetDisplayName() const override;
    virtual FText GetDescription() const override;
    virtual UTexture2D* GetIcon() const override;
    virtual void Apply(AActor* PlayerActor) override;
    int32 GetSkillID() const { return SkillID; }


private:
    // ★ 핵심: ID만 들고 있음
    int32 SkillID = INDEX_NONE;

    // UI 표시용 캐시
    FText SkillName;

    FText SkillDesc;
    UPROPERTY()
    UTexture2D* SkillIcon = nullptr;

    // UI 위젯에 바인딩할 변수들 추가
    FText ChoiceDescription;

    int32 TargetLevel;


public:

    UPROPERTY()
    bool bIsEvolution = false;

    UPROPERTY()
    int32 TargetBaseSkillID = INDEX_NONE; // 찢어버릴 기존 무기의 ID
	
};
