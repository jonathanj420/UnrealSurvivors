// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStageSlotWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UDEStageSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼에 클릭 이벤트 연결
    if (Btn_Slot)
    {
        Btn_Slot->OnClicked.AddDynamic(this, &UDEStageSlotWidget::OnButtonClicked);
    }
}

void UDEStageSlotWidget::InitSlot(const FDEStageRow& InStageRow)
{
    // 내 스테이지 ID 저장
    MyStageID = InStageRow.StageID;

    // 맵 이름 텍스트 세팅
    if (Txt_StageName)
    {
        Txt_StageName->SetText(InStageRow.StageName);
    }

}

void UDEStageSlotWidget::OnButtonClicked()
{
    // 버튼이 눌리면, "내 ID(MyStageID)가 클릭됐어요!" 하고 알람(Delegate) 방송!
    if (OnSlotClicked.IsBound())
    {
        OnSlotClicked.Broadcast(MyStageID);
    }
}
