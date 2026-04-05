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
class UDEShopWidget;

/**
 * 메인 메뉴 전체를 관리하는 위젯 클래스입니다.
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEMainMenuWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    // [UI 구성 요소]
    UPROPERTY(meta = (BindWidget))
    UWidgetSwitcher* Switcher_Main;

    // [0번 화면: 타이틀]
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_StartGame;
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_OpenShop; // 상점 열기 버튼 추가
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_QuitGame;

    // [1번 화면: 캐릭터 선택]
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_SelectCharacter;
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_BackToTitle;

    UPROPERTY(EditDefaultsOnly, Category = "Data")
    UDataTable* CharacterDataTable;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UDECharacterSlotWidget> CharacterSlotClass;

    UPROPERTY(meta = (BindWidget))
    UWrapBox* WrapBox_CharacterList;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* Txt_SelectedCharName;

    UPROPERTY(meta = (BindWidgetOptional))
    UTextBlock* Txt_SelectedCharDesc;

    UPROPERTY(meta = (BindWidgetOptional))
    UImage* Img_SelectedCharPortrait;

    int32 SelectedCharacterID = -1;

    // [2번 화면: 스테이지 선택]
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_EnterGame;
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_BackToCharacter;

    UPROPERTY(EditDefaultsOnly, Category = "Data")
    UDataTable* StageDataTable;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UDEStageSlotWidget> StageSlotClass;

    UPROPERTY(meta = (BindWidget))
    UScrollBox* ScrollBox_StageList;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_SelectedStageName;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_SelectedStageDesc;

    UPROPERTY(meta = (BindWidget))
    UImage* Img_SelectedStageThumbnail;

    FName SelectedStageLevelName;

    // [3번 화면: 상점] - WidgetSwitcher 내부에 배치됨
    UPROPERTY(meta = (BindWidget))
    UDEShopWidget* Shop_Content;

private:
    // [이벤트 핸들러]
    UFUNCTION() void OnStartGameClicked();
    UFUNCTION() void OnOpenShopClicked(); // 상점 열기 클릭
    UFUNCTION() void OnQuitGameClicked();

    UFUNCTION() void OnSelectCharacterClicked();
    UFUNCTION() void OnBackToTitleClicked();

    UFUNCTION() void OnEnterGameClicked();
    UFUNCTION() void OnBackToCharacterClicked();

    UFUNCTION() void OnShopClosed(); // 상점 닫기 이벤트 응답

    UFUNCTION() void OnCharacterSlotClicked(int32 ClickedCharacterID);
    void GenerateCharacterSlots();

    UFUNCTION() void OnStageSlotClicked(int32 ClickedStageID);
    void GenerateStageSlots();
};
