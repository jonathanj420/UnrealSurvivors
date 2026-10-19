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
	// 적용은 하지 않는다. 부모 위젯이 LevelUpManager에 넘긴다
	if (ChoiceData)
	{
		OnSlotSelected.Broadcast(ChoiceData);
	}
}