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

    if (UDESkillManagerComponent* SkillManager = PlayerActor->FindComponentByClass<UDESkillManagerComponent>())
    {
        // =========================================================
        // ★ 여기서 꼬리표를 확인하고 스킬 매니저한테 다른 명령을 내림!
        // =========================================================
        if (bIsEvolution && TargetBaseSkillID != INDEX_NONE)
        {
            // 네가 아까 짜둔 그 완벽한 진화 함수 호출! (기존 무기 삭제 + 진화 무기 장착)
            SkillManager->EvolveSkill(TargetBaseSkillID, SkillID);
        }
        else
        {
            // 꼬리표가 없으면? 기존처럼 단순하게 레벨업!
            SkillManager->ApplySkillChoice(SkillID);
        }

        /*if (!PlayerActor || SkillID == INDEX_NONE)
            return;

        if (UDESkillManagerComponent* SkillManager =
            PlayerActor->FindComponentByClass<UDESkillManagerComponent>())
        {
            SkillManager->ApplySkillChoice(SkillID);
        }*/
    }
}