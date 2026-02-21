// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DEGameHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 캐릭터와 연결해서 델리게이트 구독하는 함수
	void BindCharacterStat(class ADECharacterBase* Character);

	// 값을 갱신하는 함수
	void UpdateHP(float CurrentHP, float MaxHP);
	void UpdateExp(float CurrentExp, float MaxExp);
	void UpdateTime(float TimeSeconds);
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	int32 LastSeconds = -1;
protected:
	// --- [1. 에디터 바인딩] ---
	// 이름 꼭 맞춰야 합니다!
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_HP;  // hp

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_Exp; // exp

protected:
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Text_Time;


	UPROPERTY(meta = (BindWidget))
	class UDESkillInventoryWidget* WBP_SkillInventoryWidget;
	UPROPERTY(meta = (BindWidget))
	class UDEAccInventoryWidget* WBP_AccInventoryWidget;

};
