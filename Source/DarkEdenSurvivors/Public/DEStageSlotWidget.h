// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DEGameTypes.h"
#include "DEStageSlotWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStageSlotClicked, int32, ClickedStageID);

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEStageSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 메인 메뉴가 데이터를 던져주면, 맵 이름을 세팅하는 함수
    void InitSlot(const FDEStageRow& InStageData);

    // 메인 메뉴에서 구독(Bind)할 수 있게 열어둡니다.
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnStageSlotClicked OnSlotClicked;

protected:
    virtual void NativeConstruct() override;

private:
    // ==========================================
    // UI 컴포넌트 (위젯 블루프린트와 연결!)
    // ==========================================
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Slot;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_StageName; // "에슬라니아 시내" 글씨가 들어갈 곳

    // 내가 몇 번 맵 슬롯인지 기억해둘 변수
    int32 MyStageID;

    // 버튼이 눌렸을 때 실행될 내부 함수
    UFUNCTION()
    void OnButtonClicked();
	
};
