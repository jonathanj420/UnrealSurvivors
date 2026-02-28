// Fill out your copyright notice in the Description page of Project Settings.


#include "DECharacterSlotWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UDECharacterSlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 버튼에 클릭 이벤트 연결
    if (Btn_Slot)
    {
        Btn_Slot->OnClicked.AddDynamic(this, &UDECharacterSlotWidget::OnButtonClicked);
    }
}

void UDECharacterSlotWidget::InitSlot(const FDECharacterRow& InRowData)
{
    // 내 ID 저장
    MyCharacterID = InRowData.CharacterID;

    // 1. 이름 텍스트 세팅
    if (Txt_Name)
    {
        Txt_Name->SetText(InRowData.CharacterName);
    }

    // 2. 초상화 이미지 세팅
    if (Img_Portrait && InRowData.Portrait)
    {
        Img_Portrait->SetBrushFromTexture(InRowData.Portrait);
    }
}

void UDECharacterSlotWidget::OnButtonClicked()
{
    // 버튼이 눌리면, "내 ID(MyCharacterID)가 클릭됐어요!" 하고 알람(Delegate)을 방송합니다.
    // (나중에 메인 메뉴가 이 알람을 듣고 하단 상세 설명을 바꿔줄 겁니다.)
    if (OnSlotClicked.IsBound())
    {
        OnSlotClicked.Broadcast(MyCharacterID);
        UE_LOG(LogTemp, Warning, TEXT("Charc Button Clickd"));
    }
}
