// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DarkEdenSurvivorsGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class ADarkEdenSurvivorsGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void StartPlay() override;


public:
	
	/** Constructor */
	ADarkEdenSurvivorsGameMode();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnMonsters(TSubclassOf<APawn> MonsterClass, int32 Count, FVector CenterLocation, float Radius);

};



