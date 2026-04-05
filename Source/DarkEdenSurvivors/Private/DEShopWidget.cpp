// Fill out your copyright notice in the Description page of Project Settings.

#include "DEShopWidget.h"
#include "DEShopSlotWidget.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "DEGameInstance.h"

void UDEShopWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    if (Btn_Close)
    {
        Btn_Close->OnClicked.AddDynamic(this, &UDEShopWidget::OnCloseClicked);
    }

    InitializeShop();
}

void UDEShopWidget::InitializeShop()
{
    UE_LOG(LogTemp, Warning, TEXT("Try Init Shop"));
    //if (!ShopSlotClass || !WrapBox_UpgradeList) return;
    if (!ShopSlotClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("WTF SHOP SLOT"));
        return;
    }
    if (!WrapBox_UpgradeList)
    {
        UE_LOG(LogTemp, Warning, TEXT("WTF Upgrade List"));
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Shop Opened"));

    WrapBox_UpgradeList->ClearChildren();
    SlotWidgets.Empty();

    // 강화 가능한 능력치 목록
    TArray<EDEStatType> UpgradeableStats = {
        EDEStatType::Damage,
        EDEStatType::MaxHP,
        EDEStatType::MoveSpeed,
        EDEStatType::Amount,
        EDEStatType::Greed
    };

    for (EDEStatType Stat : UpgradeableStats)
    {
        UDEShopSlotWidget* NewSlot = CreateWidget<UDEShopSlotWidget>(this, ShopSlotClass);
        if (NewSlot)
        {
            NewSlot->InitSlot(Stat);
            WrapBox_UpgradeList->AddChildToWrapBox(NewSlot);
            SlotWidgets.Add(NewSlot);
        }
    }

    RefreshShopUI();
}

void UDEShopWidget::RefreshShopUI()
{
    UDEGameInstance* GI = Cast<UDEGameInstance>(GetGameInstance());
    if (!GI) return;

    // 현재 골드 갱신
    if (Txt_TotalGold)
    {
        Txt_TotalGold->SetText(FText::AsNumber(GI->GetTotalGold()));
    }

    // 모든 슬롯 UI 갱신 (골드가 변했으므로 비용 부족 여부 체크를 위함)
    for (UDEShopSlotWidget* ShopSlot : SlotWidgets)
    {
        if (ShopSlot)
        {
            ShopSlot->RefreshUI();
        }
    }
}

void UDEShopWidget::OnCloseClicked()
{
    OnShopClosed.Broadcast();
}
