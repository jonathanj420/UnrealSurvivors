// Fill out your copyright notice in the Description page of Project Settings.


#include "DELevelUpChoice_Skill.h"
#include "DESkillManagerComponent.h"
#include "GameFramework/Actor.h"

void UDELevelUpChoice_Skill::Init(
    int32 InSkillID,
    const FText& InSkillName,
    UTexture2D* InIcon
)
{
    SkillID = InSkillID;
    SkillName = InSkillName;
    SkillIcon = InIcon;
}

FText UDELevelUpChoice_Skill::GetDisplayName() const
{
    return SkillName;
}

FText UDELevelUpChoice_Skill::GetDescription() const
{
    // 필요하면 SkillManager에서 SkillID로 설명 조회
    return FText::GetEmpty();
}

UTexture2D* UDELevelUpChoice_Skill::GetIcon() const
{
    return SkillIcon;
}

void UDELevelUpChoice_Skill::Apply(AActor* PlayerActor)
{
    if (!PlayerActor || SkillID == INDEX_NONE)
        return;

    if (UDESkillManagerComponent* SkillManager =
        PlayerActor->FindComponentByClass<UDESkillManagerComponent>())
    {
        SkillManager->ApplySkillChoice(SkillID);
    }
}