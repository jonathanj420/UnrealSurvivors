// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DEPlayerController.generated.h"


class UDEChestWidget;
class UDELevelUpChoiceBase;

// 게임을 멈추는 이유. 하나라도 남아 있으면 멈춘 상태를 유지한다
UENUM()
enum class EDEPauseReason : uint8
{
	LevelUp,
	Chest,
	PauseMenu,
	GameOver
};

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ADEPlayerController();

	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
protected:
	// 입력을 세팅하는 언리얼 기본 함수 오버라이드
	virtual void SetupInputComponent() override;


private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UDELevelUpWidget> LevelUpWidgetClass;
	UPROPERTY()
	class UDELevelUpWidget* LevelUpWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UDEGameHUDWidget> GameHUDWidgetClass;
	UPROPERTY()
	class UDEGameHUDWidget* GameHUDWidget;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UDESkillInventoryWidget> SkillInventoryWidgetClass;
	UPROPERTY()
	class UDESkillInventoryWidget* SkillInventoryWidget;

	// 에디터에서 할당할 상자 UI 블루프린트 클래스 (WBP_ChestWidget)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDEChestWidget> ChestWidgetClass;

	// 생성된 상자 UI 인스턴스
	UPROPERTY()
	UDEChestWidget* ChestWidget;

	

public:
	// SetPause, 입력 모드, 커서는 이 두 함수만 바꾼다
	void AddPauseReason(EDEPauseReason Reason);
	void RemovePauseReason(EDEPauseReason Reason);

	// ESC 메뉴 껐다 켜기. 레벨업, 상자, 게임오버로 멈춰 있을 때는 열지 않는다
	UFUNCTION(BlueprintCallable, Category = "UI")
	void TogglePauseMenu();

	void ClosePauseMenu();

protected:
	// 에디터에서 우리가 만든 WBP_PauseMenu를 끼워넣을 슬롯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> PauseMenuClass;

private:
	// 현재 화면에 떠있는 일시정지 메뉴 인스턴스
	UPROPERTY()
	class UUserWidget* PauseMenuInstance;

	TSet<EDEPauseReason> PauseReasons;
	void ApplyPauseState();

	// 레벨업과 상자 큐는 폰의 LevelUpManager가 관리하고, 컨트롤러는 신호를 받아 표시만 한다
	void ShowLevelUpChoices(const TArray<UDELevelUpChoiceBase*>& Choices);
	void HandleLevelUpClosed();

	void ShowChestWidget(const TArray<UDELevelUpChoiceBase*>& Rewards);
	void HandleChestClosed();

	class UDELevelUpManagerComponent* GetPawnLevelUpManager() const;

};
