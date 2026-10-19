// Fill out your copyright notice in the Description page of Project Settings.


#include "DELevelUpWidget.h"
#include "DELevelUpManagerComponent.h"
#include "DELevelUpChoiceSlotWidget.h" // 슬롯 헤더 포함
#include "DELevelUpChoiceBase.h"
#include "Components/VerticalBox.h"

void UDELevelUpWidget::InitWidget(UDELevelUpManagerComponent* InLevelUpManager)
{
    LevelUpManager = InLevelUpManager;
}
void UDELevelUpWidget::SetChoices(const TArray<UDELevelUpChoiceBase*>& Choices)
{
	if (!ChoiceContainer) return;
	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SlotWidgetClass is NOT set in WBP_LevelUpWidget!"));
		return;
	}

	// 1. 이전 레벨업의 슬롯 제거
	ChoiceContainer->ClearChildren();

	// 2. 슬롯 생성 및 배치
	for (UDELevelUpChoiceBase* Choice : Choices)
	{
		// 위젯 생성 (CreateWidget)
		UDELevelUpChoiceSlotWidget* SlotWidget = CreateWidget<UDELevelUpChoiceSlotWidget>(this, SlotWidgetClass);

		if (SlotWidget)
		{
			// 데이터 주입 (이름, 아이콘 등 세팅)
			SlotWidget->InitSlot(Choice);

			// 이벤트 연결: "슬롯이 클릭되면 -> OnOptionSelected 실행해라"
			SlotWidget->OnSlotSelected.AddDynamic(this, &UDELevelUpWidget::OnOptionSelected);

			// 화면(HorizontalBox)에 추가
			ChoiceContainer->AddChild(SlotWidget);
		}
	}
}

void UDELevelUpWidget::OnOptionSelected(UDELevelUpChoiceBase* Choice)
{
	if (LevelUpManager)
	{
		LevelUpManager->ApplyChoice(Choice);
	}
}

