// Fill out your copyright notice in the Description page of Project Settings.


#include "DEChestWidget.h"
#include "DELevelUpChoiceBase.h"
#include "DEChestRewardSlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Button.h"
#include "DELevelUpManagerComponent.h"

void UDEChestWidget::NativeConstruct(){

    Super::NativeConstruct();

}

void UDEChestWidget::NativeOnInitialized()
{
    // 부모 함수 호출 (void 반환이므로 체크할 필요 없이 그냥 실행)
    Super::NativeOnInitialized();

    // 버튼 델리게이트 바인딩
    if (Btn_Close)
    {
        Btn_Close->OnClicked.AddDynamic(this, &UDEChestWidget::ApplyAllRewardsAndClose);
        Btn_Close->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UDEChestWidget::SetupChestRewards(const TArray<UDELevelUpChoiceBase*>& InRewards)
{
    // 1. 데이터만 캐싱해둠. (아직 화면에 안 그림!)
    ChestRewards = InRewards;

    // 2. 가로 박스 초기화 (안 보이게 비워둠)
    if (Box_Rewards)
    {
        Box_Rewards->ClearChildren();
    }

    // 상자가 연달아 열리면 위젯을 재사용하므로, 이전 상자에서 켜진 닫기 버튼을 다시 숨긴다
    if (Btn_Close)
    {
        Btn_Close->SetVisibility(ESlateVisibility::Collapsed);
    }

    // 3. 상태를 '클릭 대기'로 변경하고 등장 애니메이션 실행
    CurrentState = EChestWidgetState::WaitClick;
    PlayChestAppearAnimation();
}

FReply UDEChestWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    // 현재 '클릭 대기' 상태일 때만 반응함
    if (CurrentState == EChestWidgetState::WaitClick)
    {
        // 상태를 '흔들리는 중'으로 바꾸고 연출 시작
        CurrentState = EChestWidgetState::Shaking;
        PlayChestShakeAnimation();

        return FReply::Handled(); // 이벤트 소비! (더 이상 클릭 안 먹게 함)
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UDEChestWidget::RevealRewards()
{
    if (CurrentState == EChestWidgetState::RewardsShown) return; // 중복 방지

    if (RewardSlotClass)
    {
        for (UDELevelUpChoiceBase* Reward : ChestRewards)
        {
            UDEChestRewardSlotWidget* NewSlot = CreateWidget<UDEChestRewardSlotWidget>(this, RewardSlotClass);
            if (NewSlot)
            {
                NewSlot->InitSlot(Reward);
                if (Box_Rewards)
                {
                    UHorizontalBoxSlot* HBoxSlot = Box_Rewards->AddChildToHorizontalBox(NewSlot);
                    if (HBoxSlot)
                    {
                        HBoxSlot->SetPadding(FMargin(10.0f, 0.0f, 10.0f, 0.0f));
                        HBoxSlot->SetHorizontalAlignment(HAlign_Center);
                        HBoxSlot->SetVerticalAlignment(VAlign_Center);
                    }
                }
            }
        }
    }
    if (Btn_Close)
    {
        Btn_Close->SetVisibility(ESlateVisibility::Visible);
    }
    CurrentState = EChestWidgetState::RewardsShown;
}

void UDEChestWidget::ApplyAllRewardsAndClose()
{
    if (!LevelUpManager)
    {
        UE_LOG(LogTemp, Error, TEXT("[ChestWidget] LevelUpManager is not set"));
        return;
    }

    LevelUpManager->ClaimChestRewards();
}