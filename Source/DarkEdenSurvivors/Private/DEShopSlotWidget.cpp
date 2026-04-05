// Fill out your copyright notice in the Description page of Project Settings.

#include "DEShopSlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "DEGameInstance.h"

void UDEShopSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Btn_Upgrade)
    {
        Btn_Upgrade->OnClicked.AddDynamic(this, &UDEShopSlotWidget::OnUpgradeClicked);
    }
}

void UDEShopSlotWidget::InitSlot(EDEStatType InStatType)
{
    TargetStatType = InStatType;
    RefreshUI();
}

void UDEShopSlotWidget::RefreshUI()
{
    UDEGameInstance* GI = Cast<UDEGameInstance>(GetGameInstance());
    if (!GI) return;

    // 1. 이름 설정
    if (Txt_StatName)
    {
        Txt_StatName->SetText(GetStatDisplayName(TargetStatType));
    }

    // 2. 현재 레벨 표시
    if (Txt_CurrentLevel)
    {
        int32 Level = GI->GetUpgradeLevel(TargetStatType);
        Txt_CurrentLevel->SetText(FText::Format(FText::FromString(TEXT("Lv.{0}")), FText::AsNumber(Level)));
    }

    // 3. 강화 비용 표시
    if (Txt_UpgradeCost)
    {
        int32 Cost = GI->GetUpgradeCost(TargetStatType);
        Txt_UpgradeCost->SetText(FText::AsNumber(Cost));

        // 골드 부족 시 버튼 비활성화 (선택 사항)
        if (Btn_Upgrade)
        {
            Btn_Upgrade->SetIsEnabled(GI->GetTotalGold() >= Cost);
        }
    }
}

void UDEShopSlotWidget::OnUpgradeClicked()
{
    UDEGameInstance* GI = Cast<UDEGameInstance>(GetGameInstance());
    if (!GI) return;

    // 강화 시도
    if (GI->TryLevelUpStat(TargetStatType))
    {
        // 강화 성공 시 효과음 등 처리 (필요시)
        
        // 현재 슬롯 갱신
        RefreshUI();

        // TODO: 전체 골드 텍스트 갱신을 위해 상점 메인 위젯에 알림 필요
        // (간단히 모든 슬롯을 갱신하도록 상점 위젯을 참조할 수도 있음)
    }
}

FText UDEShopSlotWidget::GetStatDisplayName(EDEStatType StatType) const
{
    switch (StatType)
    {
    case EDEStatType::Damage: return FText::FromString(TEXT("공격력"));
    case EDEStatType::MaxHP: return FText::FromString(TEXT("최대 체력"));
    case EDEStatType::MoveSpeed: return FText::FromString(TEXT("이동 속도"));
    case EDEStatType::Amount: return FText::FromString(TEXT("발사체 수"));
    case EDEStatType::Greed: return FText::FromString(TEXT("골드 획득"));
    default: return FText::FromString(TEXT("미정"));
    }
}
