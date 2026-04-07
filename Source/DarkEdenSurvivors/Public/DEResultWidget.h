// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "DEResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 외부(GameMode)에서 데이터를 넣어주는 함수
	UFUNCTION(BlueprintCallable, Category = "Result")
	void SetResultData(int32 EarnedGold, int32 KillCount, float DamageDealt, float SurviveTime);

protected:
	// --- 버튼 클릭 이벤트 ---
	UFUNCTION()
	void OnRetryClicked();

	UFUNCTION()
	void OnMainMenuClicked();

protected:
	// --- WBP 바인딩 변수 (이름 꼭 맞춰야 함!) ---

	// 1. 텍스트
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_GoldEarned;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_KillCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_DamageDealt;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_SurviveTime;

	// 2. 버튼
	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Retry;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_MainMenu;

};
