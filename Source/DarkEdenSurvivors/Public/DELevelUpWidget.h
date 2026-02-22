// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DELevelUpWidget.generated.h"


class UVerticalBox;
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
    UVerticalBox* ChoiceContainer;

    // [설정] 슬롯으로 사용할 위젯 BP 클래스 (에디터에서 할당 필요)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UDELevelUpChoiceSlotWidget> SlotWidgetClass;

private:
    UPROPERTY()
    UDELevelUpManagerComponent* LevelUpManager;


};
