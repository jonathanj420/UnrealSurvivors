// Fill out your copyright notice in the Description page of Project Settings.


#include "DELevelUpWidget.h"
#include "DESkillManagerComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "DEPlayerController.h"

void UDELevelUpWidget::InitWidget(UDESkillManagerComponent* InSkillManager)
{
    SkillManager = InSkillManager;
}

void UDELevelUpWidget::NativeConstruct()
{
    Super::NativeConstruct();

    Selected = SkillManager->GetRandomSkillChoices(3);

    Txt1->SetText(FText::FromString(Selected[0]->SkillName));
    Txt2->SetText(FText::FromString(Selected[1]->SkillName));
    Txt3->SetText(FText::FromString(Selected[2]->SkillName));

    Btn1->OnClicked.AddDynamic(this, &UDELevelUpWidget::OnSelect1);
    Btn2->OnClicked.AddDynamic(this, &UDELevelUpWidget::OnSelect2);
    Btn3->OnClicked.AddDynamic(this, &UDELevelUpWidget::OnSelect3);
}

void UDELevelUpWidget::OnSelect1()
{
    SkillManager->ApplySkillChoice(Selected[0]->SkillID);
    if (auto PC = Cast<ADEPlayerController>(GetOwningPlayer()))
    {
        PC->ResumeGame();
    }
    RemoveFromParent();
}
void UDELevelUpWidget::OnSelect2()
{
    SkillManager->ApplySkillChoice(Selected[1]->SkillID);
    if (auto PC = Cast<ADEPlayerController>(GetOwningPlayer()))
    {
        PC->ResumeGame();
    }
    RemoveFromParent();
}
void UDELevelUpWidget::OnSelect3()
{
    SkillManager->ApplySkillChoice(Selected[2]->SkillID);
    if (auto PC = Cast<ADEPlayerController>(GetOwningPlayer()))
    {
        PC->ResumeGame();
    }
    RemoveFromParent();
}
