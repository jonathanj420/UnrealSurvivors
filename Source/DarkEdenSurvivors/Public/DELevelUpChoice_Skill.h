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
    void Init(
        int32 InSkillID,
        const FText& InSkillName,
        UTexture2D* InIcon
    );

    // ===== UDELevelUpChoice =====
    virtual FText GetDisplayName() const override;
    virtual FText GetDescription() const override;
    virtual UTexture2D* GetIcon() const override;
    virtual void Apply(AActor* PlayerActor) override;

private:
    // ★ 핵심: ID만 들고 있음
    int32 SkillID = INDEX_NONE;

    // UI 표시용 캐시
    FText SkillName;
    UPROPERTY()
    UTexture2D* SkillIcon = nullptr;
	
};
