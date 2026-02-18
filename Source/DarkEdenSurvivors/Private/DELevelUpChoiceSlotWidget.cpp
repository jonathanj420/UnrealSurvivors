// Fill out your copyright notice in the Description page of Project Settings.


#include "DELevelUpChoiceSlotWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UDELevelUpChoiceSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Select)
	{
		Btn_Select->OnClicked.AddDynamic(this, &UDELevelUpChoiceSlotWidget::OnSlotClicked);
	}
}

void UDELevelUpChoiceSlotWidget::InitSlot(UDELevelUpChoiceBase* InChoice)
{
	ChoiceData = InChoice;

	if (ChoiceData)
	{
		// 1. 텍스트 설정
		if (Txt_Name) Txt_Name->SetText(ChoiceData->GetDisplayName());
		if (Txt_Desc) Txt_Desc->SetText(ChoiceData->GetDescription());

		// 2. 아이콘 설정
		if (Img_Icon)
		{
			UTexture2D* IconTexture = ChoiceData->GetIcon();
			if (IconTexture)
			{
				Img_Icon->SetBrushFromTexture(IconTexture);
				Img_Icon->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				// 아이콘 없으면 숨기거나 기본 이미지
				Img_Icon->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UDELevelUpChoiceSlotWidget::OnSlotClicked()
{
	if (ChoiceData)
	{
		// ★ 핵심: 데이터 객체에게 "니 능력 적용해!" 라고 명령 (커맨드 패턴)
		// UI는 이게 스킬인지 악세서리인지 몰라도 됨
		ChoiceData->Apply(GetOwningPlayerPawn());

		// ★ 부모(메인 위젯)에게 알림 -> 부모가 창 닫고 게임 재개
		if (OnSlotSelected.IsBound())
		{
			OnSlotSelected.Broadcast();
		}
	}
}