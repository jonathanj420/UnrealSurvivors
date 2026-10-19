// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillInventoryWidget.h"
#include "DESkillManagerComponent.h"
#include "DESkillSlotWidget.h"
#include "Components/PanelWidget.h"

UDESkillInventoryWidget::UDESkillInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    bHasScriptImplementedTick = false;
}

void UDESkillInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

}

void UDESkillInventoryWidget::InitializeSkillWidget(APawn* NewPawn)
{
    if (!NewPawn) return;

    // 기존 연결 끊기 (혹시 재연결일 수 있으니)
    if (SkillManager) SkillManager->OnSkillUpdated.RemoveAll(this);

    // 컴포넌트 찾기
    SkillManager = NewPawn->FindComponentByClass<UDESkillManagerComponent>();

    // 획득, 레벨업, 진화 모두 OnSkillUpdated로 온다
    if (SkillManager)
    {
        SkillManager->OnSkillUpdated.AddUObject(this, &UDESkillInventoryWidget::OnSkillUpdated);
    }

    // 바로 갱신
    Refresh();
}

void UDESkillInventoryWidget::Refresh()
{
    if (!SkillSlotPanel || !SkillManager) return;

    SkillSlotPanel->ClearChildren();

    const TArray<int32>& SkillIDs = SkillManager->GetOwnedSkillIDs();

    for (int32 SkillID : SkillIDs)
    {
        int32 Level = SkillManager->GetSkillLevel(SkillID);
        const FDESkillRow* Row = SkillManager->GetSkillRow(SkillID);
        if (!Row) continue;

        FText SkillName = Row->SkillName;

        UDESkillSlotWidget* SkillSlot =
            CreateWidget<UDESkillSlotWidget>(this, SkillSlotWidgetClass);

        SkillSlot->SetSkill(SkillName, Level,Row->SkillIcon);
        SkillSlotPanel->AddChild(SkillSlot);
        UE_LOG(LogTemp, Error, TEXT("REFRESHED, CURRENT SKILL : %s, LEVEL :%d"), *SkillName.ToString(),Level);
    }
}

void UDESkillInventoryWidget::OnSkillUpdated(int32 InSkillID)
{
    //can do smth w skillid maybe?

    Refresh();

}
