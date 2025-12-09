// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/PlayerState.h"
#include "ABPlayerState.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangedDelegate);

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API AABPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AABPlayerState();

	int32 GetGameScore() const;
	int32 GetGameHighScore();
	int32 GetFameScore() const; //¸í¼ºÄ¡
	int32 GetGameHighFame();
	int32 GetCharacterLevel() const;
	int32 GetCharacterIndex() const;

	float GetExpRatio() const;
	bool AddExp(int32 IncomeExp);
	void AddGameScore();
	FString SaveSlotName;




	void InitPlayerData();
	void SavePlayerData();

	FOnPlayerStateChangedDelegate OnPlayerStateChanged;


protected:
	UPROPERTY(Transient)
	int32 GameScore;

	UPROPERTY(Transient)
	int32 Fame;
	
	UPROPERTY(Transient)
	int32 GameHighScore;

	UPROPERTY(Transient)
	int32 GameHighFame;

	UPROPERTY(Transient)
	int32 CharacterLevel;

	UPROPERTY(Transient)
	int32 Exp;

	UPROPERTY(Transient)
	int32 CharacterIndex;

private:
	void SetCharacterLevel(int32 NewCharacterLevel);
	struct FABCharacterData* CurrentStatData;

	
};
