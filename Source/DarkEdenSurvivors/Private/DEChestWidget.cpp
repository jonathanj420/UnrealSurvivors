// Fill out your copyright notice in the Description page of Project Settings.


#include "DEChestWidget.h"
#include "DELevelUpChoiceBase.h"
#include "DEChestRewardSlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Button.h"
#include "DEPlayerController.h"

void UDEChestWidget::NativeConstruct(){

    Super::NativeConstruct();

}

void UDEChestWidget::NativeOnInitialized()
{
    // 부모 함수 호출 (void 반환이므로 체크할 필요 없이 그냥 실행)
    Super::NativeOnInitialized();

    // 1. 플레이어 컨트롤러 캐싱
    CachedPC = Cast<ADEPlayerController>(GetOwningPlayer());
    if (!CachedPC)
    {
        UE_LOG(LogTemp, Error, TEXT("[ChestWidget] no PlayerController"));
    }

    // 2. 버튼 델리게이트 바인딩
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

    // 3. 상태를 '클릭 대기'로 변경하고 등장 애니메이션 실행
    CurrentState = EChestWidgetState::WaitClick;
    PlayChestAppearAnimation();
    //ChestRewards = InRewards;

    //// 1. 혹시 가로 박스에 이전 쓰레기 데이터가 남아있을 수 있으니 싹 비워줌
    //if (Box_Rewards)
    //{
    //    Box_Rewards->ClearChildren();
    //}

    //// 2. 블루프린트 클래스(RewardSlotClass)가 에디터에 잘 세팅되어 있는지 확인
    //if (RewardSlotClass)
    //{
    //    // 3. 보상 배열을 돌면서 슬롯을 하나씩 C++로 쾅쾅 찍어냄!
    //    for (UDELevelUpChoiceBase* Reward : ChestRewards)
    //    {
    //        // 위젯 동적 생성!
    //        UDEChestRewardSlotWidget* NewSlot = CreateWidget<UDEChestRewardSlotWidget>(this, RewardSlotClass);
    //        if (NewSlot)
    //        {
    //            // 아까 네가 짠 그 완벽한 데이터 세팅 함수 호출
    //            NewSlot->InitSlot(Reward);

    //            // 가로 박스에 집어넣기!
    //            if (Box_Rewards)
    //            {
    //                UHorizontalBoxSlot* HBoxSlot = Box_Rewards->AddChildToHorizontalBox(NewSlot);

    //                // (선택) 슬롯 사이의 간격(Padding)이나 정렬을 C++에서 제어할 수 있음
    //                if (HBoxSlot)
    //                {
    //                    HBoxSlot->SetPadding(FMargin(10.0f, 0.0f, 10.0f, 0.0f));
    //                    HBoxSlot->SetHorizontalAlignment(HAlign_Center);
    //                    HBoxSlot->SetVerticalAlignment(VAlign_Center);
    //                }
    //            }
    //        }
    //    }
    //}
    //else
    //{
    //    UE_LOG(LogTemp, Error, TEXT("RewardSlotClass is NULL"));
    //}

    //// 4. 슬롯 렌더링 끝났으니 상자 흔드는 애니메이션 시작!
    //PlayChestOpenAnimation(ChestRewards.Num());
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
    // 내 캐릭터(Player) 찾기
    APawn* OwnerPawn = GetOwningPlayerPawn();
    if (!OwnerPawn) return;

    // 상자에서 나온 모든 보상(캡슐)들을 뜯어서 플레이어에게 일괄 적용!
    for (UDELevelUpChoiceBase* Reward : ChestRewards)
    {
        if (Reward)
        {
            Reward->Apply(OwnerPawn);
        }
    }

    // 적용 끝났으니 위젯 닫고, 일시정지(Pause) 풀기
    RemoveFromParent();

    if (CachedPC)
    {
        CachedPC->ResumeGame();
        // 마우스 커서 숨기기 등 추가 처리
    }
}