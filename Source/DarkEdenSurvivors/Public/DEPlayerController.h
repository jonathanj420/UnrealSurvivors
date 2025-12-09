// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DEPlayerController.generated.h"

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

public:
	UFUNCTION()
	void ResumeGame();
	void ShowLevelUpUI();
};
