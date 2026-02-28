// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEGameModeBase.h"
#include "DEGameMode_Lobby.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEGameMode_Lobby : public ADEGameModeBase
{
	GENERATED_BODY()

	ADEGameMode_Lobby();

protected:
	virtual void BeginPlay() override;



	
};
