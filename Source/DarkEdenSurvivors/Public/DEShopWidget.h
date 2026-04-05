// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DEStatTypes.h"
#include "DEShopWidget.generated.h"

class UWrapBox;
class UTextBlock;
class UButton;
class UDEShopSlotWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShopClosed);

/**
 * 메타 프로그레션(영구 강화) 상점 위젯입니다.
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEShopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // 상점 초기화 (강화 항목 생성)
    void InitializeShop();

    // 상점의 모든 데이터 갱신 (골드 갱신 포함)
    UFUNCTION(BlueprintCallable)
    void RefreshShopUI();

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UWrapBox* WrapBox_UpgradeList;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_TotalGold;

    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Close;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UDEShopSlotWidget> ShopSlotClass;

public:
    // 상점 닫기 이벤트 (메인 메뉴 위젯에서 구독)
    FOnShopClosed OnShopClosed;

private:
    UFUNCTION()
    void OnCloseClicked();

    // 생성된 슬롯들을 관리
    UPROPERTY()
    TArray<UDEShopSlotWidget*> SlotWidgets;
};
