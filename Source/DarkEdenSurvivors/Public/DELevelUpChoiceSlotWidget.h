// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DELevelUpChoiceBase.h"
#include "DELevelUpChoiceSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlotSelected);

class UButton;
class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDELevelUpChoiceSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 외부(메인 위젯)에서 데이터를 넣어주는 함수
	UFUNCTION(BlueprintCallable)
	void InitSlot(UDELevelUpChoiceBase* InChoice);

protected:
	UFUNCTION()
	void OnSlotClicked();

public:
	// =========================================================
	// [UI 컴포넌트 바인딩] 이름 꼭 맞춰야 함!
	// =========================================================
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Select; // 버튼 (전체 덮는 투명 버튼 추천)

	UPROPERTY(meta = (BindWidget))
	UImage* Img_Icon;    // 스킬/아이템 아이콘

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Name; // 이름

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Desc; // 설명 (데미지 증가량 등)

	// =========================================================
	// [데이터]
	// =========================================================
	UPROPERTY(BlueprintReadOnly)
	UDELevelUpChoiceBase* ChoiceData;

	// 부모가 듣게 될 이벤트
	UPROPERTY(BlueprintAssignable)
	FOnSlotSelected OnSlotSelected;
	
};
