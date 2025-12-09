// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/SaveGame.h"
#include "ABSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UABSaveGame : public USaveGame
{
	GENERATED_BODY()
	

public:
	UABSaveGame();

	UPROPERTY();
	int32 Level;

	UPROPERTY();
	int32 Exp;

	UPROPERTY();
	int32 Fame;

	UPROPERTY();
	FString PlayerName;

	UPROPERTY();
	int32 HighScore;

	UPROPERTY();
	int32 HighFame;

	UPROPERTY();
	int32 CharacterIndex;



};
