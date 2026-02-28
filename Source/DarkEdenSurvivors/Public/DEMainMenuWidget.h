// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "DEMainMenuWidget.generated.h"

class UButton;
class UWidgetSwitcher;
class UWrapBox;
class UTextBlock;
class UImage;
class UScrollBox;
class UDECharacterSlotWidget;
class UDEStageSlotWidget;

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    // ==========================================
    // ==========================================
    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* Switcher_Main;

    // [0번 화면: 타이틀]
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_StartGame;
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_QuitGame;

    // [1번 화면: 캐릭터 선택]
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_SelectCharacter; // 다음으로 (초기엔 비활성화)
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_BackToTitle;

    UPROPERTY(EditDefaultsOnly, Category = "Data")
    UDataTable* CharacterDataTable;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UDECharacterSlotWidget> CharacterSlotClass;

    UPROPERTY(meta = (BindWidget))
    UWrapBox* WrapBox_CharacterList;

    // 슬롯 클릭 시 띄워줄 하단 상세 정보 UI들 (위젯 블루프린트에서 추가 필요!)
    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* Txt_SelectedCharName;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* Txt_SelectedCharDesc;

    UPROPERTY(meta = (BindWidgetOptional))
    UImage* Img_SelectedCharPortrait;

    // 현재 유저가 최종 선택한 캐릭터 ID를 기억해둘 변수
    int32 SelectedCharacterID = -1;


    // [2번 화면: 스테이지 선택]
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_EnterGame;       // 게임 진입!
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_BackToCharacter;

    // 1. 에디터에서 꽂아넣을 맵 데이터 테이블 & 붕어빵 틀
    UPROPERTY(EditDefaultsOnly, Category = "Data")
    UDataTable* StageDataTable;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UDEStageSlotWidget> StageSlotClass;

    // 2. 맵 슬롯들을 세로로 담아줄 스크롤 상자 (위젯 블루프린트와 연결!)
    UPROPERTY(meta = (BindWidget))
    UScrollBox* ScrollBox_StageList;

    // 3. 우측 상세 정보 UI들 (선택된 맵 프리뷰, 이름, 설명)
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_SelectedStageName;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_SelectedStageDesc;

    UPROPERTY(meta = (BindWidget))
    UImage* Img_SelectedStageThumbnail;

    // ★ 4. 유저가 최종 선택한 맵의 '진짜 레벨 이름'을 기억해둘 변수
    FName SelectedStageLevelName;


    UFUNCTION() void OnStartGameClicked();
    UFUNCTION() void OnQuitGameClicked();

    UFUNCTION() void OnSelectCharacterClicked();
    UFUNCTION() void OnBackToTitleClicked();

    UFUNCTION() void OnEnterGameClicked();
    UFUNCTION() void OnBackToCharacterClicked();

    UFUNCTION() void OnCharacterSlotClicked(int32 ClickedCharacterID);
    void GenerateCharacterSlots();

    UFUNCTION()
    void OnStageSlotClicked(int32 ClickedStageID);

    void GenerateStageSlots();
};
