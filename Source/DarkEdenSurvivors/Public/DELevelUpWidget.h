// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DELevelUpWidget.generated.h"


class UVerticalBox;
class UDELevelUpChoiceSlotWidget;
class UDELevelUpManagerComponent;
class UDELevelUpChoiceBase;

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

    // 선택지는 LevelUpManager가 만들어서 넘겨준다. 위젯 하나를 다음 레벨업에도 재사용한다
    void SetChoices(const TArray<UDELevelUpChoiceBase*>& Choices);

protected:
    // 선택만 매니저에 넘긴다. 창을 닫고 게임을 재개하는 건 매니저 신호를 받은 컨트롤러가 한다
    UFUNCTION()
    void OnOptionSelected(UDELevelUpChoiceBase* Choice);

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
