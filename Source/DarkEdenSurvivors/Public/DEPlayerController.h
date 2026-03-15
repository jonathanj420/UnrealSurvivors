// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DEPlayerController.generated.h"


class UDEChestWidget;
class UDELevelUpChoiceBase;


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

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;



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
	UFUNCTION()
	void ResumeGame();
	void ShowLevelUpUI();
	void ShowChestWidget(const TArray<UDELevelUpChoiceBase*>& Rewards);
};
