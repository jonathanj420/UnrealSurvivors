// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DELevelUpWidget.generated.h"


class UHorizontalBox;
class UDELevelUpChoiceSlotWidget;
class UDELevelUpManagerComponent;

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDELevelUpWidget : public UUserWidget
{

    GENERATED_BODY()

public:
    // 매니저 등록 함수
    void InitWidget(UDELevelUpManagerComponent* InManager);

protected:
    virtual void NativeConstruct() override;

    // 슬롯에서 "나 선택됐어!" 신호를 보내면 실행될 함수
    UFUNCTION()
    void OnOptionSelected();

public:
    // =========================================================
    // [UI 바인딩] 기존 버튼 3개 -> 가로 박스 1개로 변경!
    // =========================================================
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* ChoiceContainer;

    // [설정] 슬롯으로 사용할 위젯 BP 클래스 (에디터에서 할당 필요)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UDELevelUpChoiceSlotWidget> SlotWidgetClass;

private:
    UPROPERTY()
    UDELevelUpManagerComponent* LevelUpManager;

    /*GENERATED_BODY()

public:
    void InitWidget(UDELevelUpManagerComponent* InLevelUpManager);

protected:
    virtual void NativeConstruct() override;

protected:

    UPROPERTY(meta = (BindWidget))
    UButton* Btn1;
    UPROPERTY(meta = (BindWidget))
    UButton* Btn2;
    UPROPERTY(meta = (BindWidget))
    UButton* Btn3;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt1;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt2;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt3;

    UPROPERTY()
    UDELevelUpManagerComponent* LevelUpManager;

    TArray<UDELevelUpChoiceBase*> SelectedChoices;

    UFUNCTION()
    void OnSelect1();
    UFUNCTION()
    void OnSelect2();
    UFUNCTION()
    void OnSelect3();*/
};
