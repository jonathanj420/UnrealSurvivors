// Fill out your copyright notice in the Description page of Project Settings.

#include "DEMainMenuWidget.h"
#include "DECharacterSlotWidget.h"
#include "DEStageSlotWidget.h"
#include "DEShopWidget.h"
#include "DECharacterRow.h"
#include "Components/Button.h"
#include "DEGameTypes.h"
#include "Components/WidgetSwitcher.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UDEMainMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 초기 화면 설정 (0: 타이틀)
    if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(0);

    // 캐릭터 선택 버튼은 캐릭터가 선택되기 전까지 비활성화
    if (Btn_SelectCharacter) Btn_SelectCharacter->SetIsEnabled(false);

    // 버튼 이벤트 바인딩
    if (Btn_StartGame) Btn_StartGame->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnStartGameClicked);
    if (Btn_OpenShop) Btn_OpenShop->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnOpenShopClicked);
    if (Btn_QuitGame) Btn_QuitGame->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnQuitGameClicked);

    if (Btn_SelectCharacter) Btn_SelectCharacter->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnSelectCharacterClicked);
    if (Btn_BackToTitle) Btn_BackToTitle->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnBackToTitleClicked);

    if (Btn_EnterGame) Btn_EnterGame->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnEnterGameClicked);
    if (Btn_BackToCharacter) Btn_BackToCharacter->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnBackToCharacterClicked);

    // 상점 이벤트 바인딩
    if (Shop_Content)
    {
        Shop_Content->OnShopClosed.AddDynamic(this, &UDEMainMenuWidget::OnShopClosed);
    }

    // 초기화
    GenerateCharacterSlots();
    GenerateStageSlots();
}

void UDEMainMenuWidget::GenerateCharacterSlots()
{
    if (!CharacterDataTable || !CharacterSlotClass || !WrapBox_CharacterList) return;

    WrapBox_CharacterList->ClearChildren();

    TArray<FDECharacterRow*> AllCharacters;
    CharacterDataTable->GetAllRows<FDECharacterRow>(TEXT("CharacterContext"), AllCharacters);

    for (FDECharacterRow* RowData : AllCharacters)
    {
        if (RowData)
        {
            UDECharacterSlotWidget* NewSlot = CreateWidget<UDECharacterSlotWidget>(this, CharacterSlotClass);
            if (NewSlot)
            {
                NewSlot->InitSlot(*RowData);
                NewSlot->OnSlotClicked.AddDynamic(this, &UDEMainMenuWidget::OnCharacterSlotClicked);
                WrapBox_CharacterList->AddChildToWrapBox(NewSlot);
            }
        }
    }
}

void UDEMainMenuWidget::OnCharacterSlotClicked(int32 ClickedCharacterID)
{
    SelectedCharacterID = ClickedCharacterID;
    FName RowName = FName(*FString::FromInt(ClickedCharacterID));
    FDECharacterRow* FoundRow = CharacterDataTable->FindRow<FDECharacterRow>(RowName, TEXT("FindCharacter"));

    if (FoundRow)
    {
        if (Txt_SelectedCharName) Txt_SelectedCharName->SetText(FoundRow->CharacterName);
        if (Txt_SelectedCharDesc) Txt_SelectedCharDesc->SetText(FoundRow->Description);
        if (Img_SelectedCharPortrait && FoundRow->Portrait) Img_SelectedCharPortrait->SetBrushFromTexture(FoundRow->Portrait);

        if (Btn_SelectCharacter) Btn_SelectCharacter->SetIsEnabled(true);
    }
}

void UDEMainMenuWidget::GenerateStageSlots()
{
    if (!StageDataTable || !StageSlotClass || !ScrollBox_StageList) return;

    ScrollBox_StageList->ClearChildren();

    TArray<FDEStageRow*> AllStages;
    StageDataTable->GetAllRows<FDEStageRow>(TEXT("StageContext"), AllStages);

    for (FDEStageRow* StageData : AllStages)
    {
        if (StageData)
        {
            UDEStageSlotWidget* NewSlot = CreateWidget<UDEStageSlotWidget>(this, StageSlotClass);
            if (NewSlot)
            {
                NewSlot->InitSlot(*StageData);
                NewSlot->OnSlotClicked.AddDynamic(this, &UDEMainMenuWidget::OnStageSlotClicked);
                ScrollBox_StageList->AddChild(NewSlot);
            }
        }
    }
}

void UDEMainMenuWidget::OnStageSlotClicked(int32 ClickedStageID)
{
    FName RowName = FName(*FString::FromInt(ClickedStageID));
    FDEStageRow* FoundStage = StageDataTable->FindRow<FDEStageRow>(RowName, TEXT("FindStage"));

    if (FoundStage)
    {
        if (Txt_SelectedStageName) Txt_SelectedStageName->SetText(FoundStage->StageName);
        if (Txt_SelectedStageDesc) Txt_SelectedStageDesc->SetText(FoundStage->Description);
        if (Img_SelectedStageThumbnail && FoundStage->StageThumbnail)
        {
            Img_SelectedStageThumbnail->SetBrushFromTexture(FoundStage->StageThumbnail);
        }

        SelectedStageLevelName = FoundStage->LevelName;
        if (Btn_EnterGame) Btn_EnterGame->SetIsEnabled(true);
    }
}

// 화면 전환 핸들러
void UDEMainMenuWidget::OnStartGameClicked() { if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(1); }

void UDEMainMenuWidget::OnOpenShopClicked() 
{ 
    if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(3); // 상점 화면으로 전환
    if (Shop_Content) Shop_Content->RefreshShopUI(); // 최신 골드 수치 등 반영
}

void UDEMainMenuWidget::OnQuitGameClicked() { UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true); }

void UDEMainMenuWidget::OnSelectCharacterClicked() { if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(2); }
void UDEMainMenuWidget::OnBackToTitleClicked() { if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(0); }

void UDEMainMenuWidget::OnEnterGameClicked()
{
    if (SelectedStageLevelName.IsNone()) return;
    UGameplayStatics::OpenLevel(this, SelectedStageLevelName);
}

void UDEMainMenuWidget::OnBackToCharacterClicked() { if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(1); }

void UDEMainMenuWidget::OnShopClosed()
{
    if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(0); // 다시 타이틀로 복귀
}
