// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillInventoryWidget.h"
#include "DEInventoryComponent.h"
#include "DESkillManagerComponent.h"
#include "DESkillSlotWidget.h"
#include "Components/PanelWidget.h"

void UDESkillInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    APawn* Pawn = GetOwningPlayerPawn();
    if (!Pawn) return;

    Inventory = Pawn->FindComponentByClass<UDEInventoryComponent>();
    SkillManager = Pawn->FindComponentByClass<UDESkillManagerComponent>();
    check(SkillSlotWidgetClass);
    check(Inventory);
    check(SkillManager);

    if (SkillManager)
    {
        SkillManager->OnSkillUpdated.AddUObject(
            this,
            &UDESkillInventoryWidget::OnSkillUpdated
        );
    }
    //Inventory 변경 이벤트 구독
    Inventory->OnInventoryChanged.AddUObject(
        this,
        &UDESkillInventoryWidget::Refresh
    );

    Refresh();
}

void UDESkillInventoryWidget::Refresh()
{
    if (!SkillSlotPanel) return;

    SkillSlotPanel->ClearChildren();

    const TArray<int32>& SkillIDs = Inventory->GetOwnedSkillIDs();

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
