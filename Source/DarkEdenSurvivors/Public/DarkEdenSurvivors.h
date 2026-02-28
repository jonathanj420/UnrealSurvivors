// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"

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

#define ECC_MonsterChannel ECC_GameTraceChannel5