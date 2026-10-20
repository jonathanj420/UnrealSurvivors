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
	void UpdateLevel(int32 CurrentLevel);

protected:
	// HP 비율이 이 값 이하면 위험 상태 (화면 가장자리 비네트)
	UPROPERTY(EditDefaultsOnly, Category = "HUD", meta = (ClampMin = "0", ClampMax = "1"))
	float LowHPThreshold = 0.3f;

	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	bool bLowHP = false;

	// 위험 상태에 들어가거나 벗어날 때 한 번 불린다. WBP에서 비네트를 켜고 끈다
	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void OnLowHPChanged(bool bIsLowHP);

protected:
	virtual void NativeConstruct() override;
	//virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	int32 LastSeconds = -1;

	// 알람시계 손잡이
	FTimerHandle TimeUpdateTimerHandle;

	// 타이머가 1초마다 부를 함수
	UFUNCTION()
	void UpdateTimeTick();

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
	class UTextBlock* Text_Level;

	UPROPERTY(meta = (BindWidget))
	class UDESkillInventoryWidget* WBP_SkillInventoryWidget;
	UPROPERTY(meta = (BindWidget))
	class UDEAccInventoryWidget* WBP_AccInventoryWidget;

};
