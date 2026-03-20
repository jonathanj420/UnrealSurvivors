// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillSlotWidget.h"

UDESkillSlotWidget::UDESkillSlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

    bHasScriptImplementedTick = false;
}

void UDESkillSlotWidget::SetSkill(const FText& InSkillName, int32 InSkillLevel, UTexture2D* InSkillIcon)
{
	SkillName = InSkillName;
	SkillLevel = InSkillLevel;

    // --------------------
    // 텍스트 위젯 설정
    // --------------------

    if (SkillLevelText)
    {
        SkillLevelText->SetText(
            FText::Format(NSLOCTEXT("Skill", "Level", "Lv.{0}"), InSkillLevel)
        );
    }

    // --------------------
    // 이미지 위젯 설정
    // --------------------
    if (SkillIconImage)
    {
        if (InSkillIcon)
        {
            UE_LOG(LogTemp, Warning, TEXT("Icon Found: %s"), *InSkillIcon->GetName());
            SkillIconImage->SetBrushFromTexture(InSkillIcon);
            SkillIconImage->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            SkillIconImage->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    UE_LOG(LogTemp, Error, TEXT("Skill Slot UI SET for :%s"),*InSkillName.ToString());

}
