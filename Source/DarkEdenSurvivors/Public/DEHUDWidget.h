// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DEHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 캐릭터와 연결해서 델리게이트 구독하는 함수
	void BindCharacterStat(class ADECharacterBase* Character);

	// 값을 갱신하는 함수
	void UpdateHP(float CurrentHP, float MaxHP);
	void UpdateExp(float CurrentExp, float MaxExp);

protected:
	virtual void NativeConstruct() override;

protected:
	// --- [1. 에디터 바인딩] ---
	// 이름 꼭 맞춰야 합니다!
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_HP;  // hp

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_Exp; // exp

	// (선택) 레벨 텍스트도 있다면?
	// UPROPERTY(meta = (BindWidget))
	// class UTextBlock* Text_Level;

};
