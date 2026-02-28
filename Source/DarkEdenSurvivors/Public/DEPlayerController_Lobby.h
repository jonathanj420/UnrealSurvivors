// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DEPlayerController_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEPlayerController_Lobby : public APlayerController
{
	GENERATED_BODY()
	
	ADEPlayerController_Lobby();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TSubclassOf<class UDEMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY()
	class UDEMainMenuWidget* MainMenuWidget;

};
