// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	PREINIT,
	LOADING,
	READY,
	DEAD
};

/** Main log category used across the project */
DECLARE_LOG_CATEGORY_EXTERN(LogDarkEdenSurvivors, Log, All);

#define DE_LOG(Format, ...) UE_LOG(LogDarkEdenSurvivors, Display, TEXT(Format), ##__VA_ARGS__)

#define LOG_CALL() UE_LOG(LogTemp, Warning, TEXT("[CALL] %s | %s:%d | %s"), \
    ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(__FILE__), __LINE__, *GetNameSafe(this))

// DefaultEngine.ini의 DefaultChannelResponses 순서와 맞춰야 한다
#define ECC_Monster         ECC_GameTraceChannel5
#define ECC_PlayerAttack    ECC_GameTraceChannel6