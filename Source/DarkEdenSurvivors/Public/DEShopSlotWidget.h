// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DEStatTypes.h"
#include "DEShopSlotWidget.generated.h"

class UTextBlock;
class UButton;

/**
 * 개별 업그레이드 항목을 표시하는 슬롯 위젯입니다.
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEShopSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // 이 슬롯이 담당할 능력치 설정
    void InitSlot(EDEStatType InStatType);

    // 상점 메인 위젯에서 데이터 갱신 시 호출
    void RefreshUI();

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_StatName;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_CurrentLevel;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt_UpgradeCost;

    UPROPERTY(meta = (BindWidget))
    UButton* Btn_Upgrade;

    UFUNCTION()
    void OnUpgradeClicked();

private:
    EDEStatType TargetStatType;
    
    // 능력치 타입에 따른 한글 이름 반환
    FText GetStatDisplayName(EDEStatType StatType) const;
};
