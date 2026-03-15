// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DEChestRewardSlotWidget.generated.h"

class UDELevelUpChoiceBase;
class UImage;
class UTextBlock;


/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEChestRewardSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 메인 상자 위젯이 이 슬롯을 생성할 때 데이터를 밀어넣어 줄 함수!
	UFUNCTION(BlueprintCallable, Category = "Chest UI")
	void InitSlot(UDELevelUpChoiceBase* RewardData);

protected:
	// UMG에서 똑같은 이름으로 만들면 자동 연결됨
	UPROPERTY(meta = (BindWidget))
	UImage* Img_RewardIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_RewardName;

	// (선택) 레벨업인지, 진화인지 띄워줄 텍스트
	//UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	//UTextBlock* Txt_LevelInfo;

	// C++ 데이터 세팅이 끝나면, 블루프린트 쪽에 "이제 후광(빛무리) 돌려라!" 하고 알려주는 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Chest UI")
	void OnSlotRevealed();
	
};
