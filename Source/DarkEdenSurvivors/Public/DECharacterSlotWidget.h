// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DECharacterRow.h"
#include "DECharacterSlotWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSlotClicked, int32, ClickedCharacterID);


/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDECharacterSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 메인 메뉴가 데이터를 던져주면, 얼굴과 이름을 세팅하는 함수
    void InitSlot(const FDECharacterRow& InRowData);

    // 메인 메뉴에서 이 알람을 구독(Bind)할 수 있게 public으로 열어둡니다.
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCharacterSlotClicked OnSlotClicked;

protected:
    virtual void NativeConstruct() override;

private:
    // ==========================================
    // UI 컴포넌트 (위젯 블루프린트와 연결!)
    // ==========================================
    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Slot;

    UPROPERTY(meta = (BindWidget))
    UImage* Img_Portrait;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_Name;

    // 내가 몇 번 캐릭터 슬롯인지 기억해둘 변수
    int32 MyCharacterID;

    // 버튼이 눌렸을 때 내부적으로 실행될 함수
    UFUNCTION()
    void OnButtonClicked();
	
};
