// Fill out your copyright notice in the Description page of Project Settings.


#include "DELevelUpWidget.h"
#include "DELevelUpManagerComponent.h"
#include "DELevelUpChoiceSlotWidget.h" // 슬롯 헤더 포함
#include "DELevelUpChoiceBase.h"
#include "Components/HorizontalBox.h"
#include "DEPlayerController.h"

void UDELevelUpWidget::InitWidget(UDELevelUpManagerComponent* InLevelUpManager)
{
    LevelUpManager = InLevelUpManager;
}
void UDELevelUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!LevelUpManager) return;
	if (!ChoiceContainer) return;
	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("SlotWidgetClass is NOT set in WBP_LevelUpWidget!"));
		return;
	}

	// 1. 기존에 남아있던 슬롯이 있다면 제거 (리롤 기능 대비)
	ChoiceContainer->ClearChildren();

	// 2. 매니저에게 선택지 3개(혹은 4개) 받아오기
	// (함수 이름은 님이 만든 Manager에 맞춰서 GetRandomChoices or GenerateChoices 사용)
	TArray<UDELevelUpChoiceBase*> Choices = LevelUpManager->GenerateChoices(3);

	// 3. 슬롯 생성 및 배치
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

void UDELevelUpWidget::OnOptionSelected()
{
	// ★ 중요: Apply()는 이미 SlotWidget 안에서 실행되었습니다.
	// 여기서는 창을 닫고 게임을 재개하는 역할만 합니다.

	if (auto PC = Cast<ADEPlayerController>(GetOwningPlayer()))
	{
		PC->ResumeGame();
	}

	RemoveFromParent();
}

//
//void UDELevelUpWidget::OnSelect1()
//{
//    LevelUpManager->ApplyChoice(SelectedChoices[0]);
//
//    if (auto PC = Cast<ADEPlayerController>(GetOwningPlayer()))
//    {
//        PC->ResumeGame();
//    }
//
//    RemoveFromParent();
//}
//void UDELevelUpWidget::OnSelect2()
//{
//    LevelUpManager->ApplyChoice(SelectedChoices[1]);
//
//    if (auto PC = Cast<ADEPlayerController>(GetOwningPlayer()))
//    {
//        PC->ResumeGame();
//    }
//
//    RemoveFromParent();
//}
//void UDELevelUpWidget::OnSelect3()
//{
//    LevelUpManager->ApplyChoice(SelectedChoices[2]);
//
//    if (auto PC = Cast<ADEPlayerController>(GetOwningPlayer()))
//    {
//        PC->ResumeGame();
//    }
//
//    RemoveFromParent();
//}
