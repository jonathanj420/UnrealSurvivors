// Fill out your copyright notice in the Description page of Project Settings.


#include "DEMainMenuWidget.h"
#include "DECharacterSlotWidget.h"
#include "DEStageSlotWidget.h"
#include "DECharacterRow.h" // 데이터 테이블 구조체
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

    // 1. 초기 화면 세팅: 무조건 0번(타이틀) 띄우기
    if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(0);

    // 2. 캐릭터를 고르기 전까진 '다음으로' 버튼 비활성화 (뱀서 디테일!)
    if (Btn_SelectCharacter) Btn_SelectCharacter->SetIsEnabled(false);

    // 3. 버튼 클릭 이벤트 연결
    if (Btn_StartGame) Btn_StartGame->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnStartGameClicked);
    if (Btn_QuitGame) Btn_QuitGame->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnQuitGameClicked);

    if (Btn_SelectCharacter) Btn_SelectCharacter->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnSelectCharacterClicked);
    if (Btn_BackToTitle) Btn_BackToTitle->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnBackToTitleClicked);

    if (Btn_EnterGame) Btn_EnterGame->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnEnterGameClicked);
    if (Btn_BackToCharacter) Btn_BackToCharacter->OnClicked.AddDynamic(this, &UDEMainMenuWidget::OnBackToCharacterClicked);

    // 4. 캐릭터 목록 슬롯 생성 실행!
    GenerateCharacterSlots();
    GenerateStageSlots();
}

void UDEMainMenuWidget::GenerateCharacterSlots()
{
    UE_LOG(LogTemp, Warning, TEXT("Try Generate Char Slots"));
    //if (!CharacterDataTable || !CharacterSlotClass || !WrapBox_CharacterList) return;
    if (!CharacterDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("NO CHARAC DT"));
        return;
    }
    if (!CharacterSlotClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("NO CHARAC SLOTS"));
        return;
    }

    if (!WrapBox_CharacterList)
    {
        UE_LOG(LogTemp, Warning, TEXT("NO WRAP "));
        return;
    }



    UE_LOG(LogTemp, Warning, TEXT("Reqments Passed"));
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
                // 슬롯이 클릭됐을 때 내 함수(OnCharacterSlotClicked)가 실행되도록 연결
                NewSlot->OnSlotClicked.AddDynamic(this, &UDEMainMenuWidget::OnCharacterSlotClicked);
                WrapBox_CharacterList->AddChildToWrapBox(NewSlot);
                UE_LOG(LogTemp, Warning, TEXT("Char Slot Goood"));
            }
        }
    }
}

// ==========================================
// ★ 핵심: 슬롯 클릭 시 하단 정보창 업데이트
// ==========================================
void UDEMainMenuWidget::OnCharacterSlotClicked(int32 ClickedCharacterID)
{
    // 선택한 ID 기억하기
    SelectedCharacterID = ClickedCharacterID;

    // 데이터 테이블에서 이 ID에 해당하는 줄(Row) 찾기
    // (보통 Row Name을 ID를 String으로 변환해서 씁니다. 에디터에서 행 이름을 1, 2, 3.. 으로 지어주세요!)
    FName RowName = FName(*FString::FromInt(ClickedCharacterID));
    FDECharacterRow* FoundRow = CharacterDataTable->FindRow<FDECharacterRow>(RowName, TEXT("FindCharacter"));

    if (FoundRow)
    {

        UE_LOG(LogTemp, Warning, TEXT("Charc Row arimasuyo"));
        // 하단 상세 정보 UI 업데이트
        if (Txt_SelectedCharName) Txt_SelectedCharName->SetText(FoundRow->CharacterName);
        if (Txt_SelectedCharDesc) Txt_SelectedCharDesc->SetText(FoundRow->Description);
        if (Img_SelectedCharPortrait && FoundRow->Portrait) Img_SelectedCharPortrait->SetBrushFromTexture(FoundRow->Portrait);

        // 캐릭터를 골랐으니 드디어 '다음으로' 버튼 활성화!
        if (Btn_SelectCharacter) Btn_SelectCharacter->SetIsEnabled(true);
    }
}

void UDEMainMenuWidget::GenerateStageSlots()
{
    UE_LOG(LogTemp, Warning, TEXT("Try Generate Stage SLots"));
    // 안전 검사
    //if (!StageDataTable || !StageSlotClass || !ScrollBox_StageList) return;
    if (!StageDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("NO StageDataTable DT"));
        return;
    }
    if (!StageSlotClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("NO StageSlotClass SLOTS"));
        return;
    }

    if (!ScrollBox_StageList)
    {
        UE_LOG(LogTemp, Warning, TEXT("NO ScrollBox_StageList "));
        return;
    }


    ScrollBox_StageList->ClearChildren();

    // 데이터 테이블 긁어오기
    TArray<FDEStageRow*> AllStages; // (구조체 이름을 Row로 하셨다면 FDEStageRow)
    StageDataTable->GetAllRows<FDEStageRow>(TEXT("StageContext"), AllStages);

    // 슬롯 쫙 스폰해서 스크롤 상자에 꽂아넣기!
    for (FDEStageRow* StageData : AllStages)
    {
        if (StageData)
        {
            UDEStageSlotWidget* NewSlot = CreateWidget<UDEStageSlotWidget>(this, StageSlotClass);
            if (NewSlot)
            {
                NewSlot->InitSlot(*StageData);
                // 슬롯이 눌리면 내 함수(OnStageSlotClicked)가 실행되도록 연결!
                NewSlot->OnSlotClicked.AddDynamic(this, &UDEMainMenuWidget::OnStageSlotClicked);

                ScrollBox_StageList->AddChild(NewSlot);
            }
        }
    }
}

// 맵 리스트 중 하나를 클릭했을 때!
void UDEMainMenuWidget::OnStageSlotClicked(int32 ClickedStageID)
{
    // 데이터 테이블에서 이 ID에 해당하는 줄 찾기
    FName RowName = FName(*FString::FromInt(ClickedStageID));
    FDEStageRow* FoundStage = StageDataTable->FindRow<FDEStageRow>(RowName, TEXT("FindStage"));

    if (FoundStage)
    {
        // 1. 우측 상세 정보 UI 업데이트
        if (Txt_SelectedStageName) Txt_SelectedStageName->SetText(FoundStage->StageName);
        if (Txt_SelectedStageDesc) Txt_SelectedStageDesc->SetText(FoundStage->Description);
        if (Img_SelectedStageThumbnail && FoundStage->StageThumbnail)
        {
            Img_SelectedStageThumbnail->SetBrushFromTexture(FoundStage->StageThumbnail);
        }

        // 2. ★ 인게임으로 넘어갈 때 쓸 진짜 맵 이름(LevelName) 메모해두기!
        SelectedStageLevelName = FoundStage->LevelName;

        // 전투 진입 버튼 활성화 (캐릭터 때랑 똑같이 초기엔 비활성화해두는 걸 추천)
        if (Btn_EnterGame) Btn_EnterGame->SetIsEnabled(true);
    }
}

// ==========================================
// 화면 전환 및 게임 종료 로직
// ==========================================
void UDEMainMenuWidget::OnStartGameClicked() { if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(1); }
void UDEMainMenuWidget::OnQuitGameClicked() { UKismetSystemLibrary::QuitGame(this, GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, true); }

void UDEMainMenuWidget::OnSelectCharacterClicked() { if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(2); }
void UDEMainMenuWidget::OnBackToTitleClicked() { if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(0); }

void UDEMainMenuWidget::OnEnterGameClicked()
{
    // 아무 맵도 안 골랐으면(이름이 비어있으면) 막기
    if (SelectedStageLevelName.IsNone()) return;

    // TODO: GameInstance에 선택한 캐릭터 ID 저장하는 로직 필요

    // ★ 하드코딩 탈출! 유저가 고른 맵 이름으로 완벽하게 이동!
    UGameplayStatics::OpenLevel(this, SelectedStageLevelName);
}
void UDEMainMenuWidget::OnBackToCharacterClicked() { if (Switcher_Main) Switcher_Main->SetActiveWidgetIndex(1); }