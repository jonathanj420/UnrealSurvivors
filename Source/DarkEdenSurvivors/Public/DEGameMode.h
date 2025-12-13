// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "GameFramework/GameModeBase.h"
#include "DEGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ADEGameMode();
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

public:
    float GetElapsedTime() const { return ElapsedTime; }
    void RegisterMonsterSpawnManager(class ADEMonsterSpawnManager* InManager);
    class ADEMonsterSpawnManager* GetMonsterSpawnManager() { return MonsterSpawnManager; }


    int32 GetSpawnLimit() const { return SpawnLimit; }

protected:
    UPROPERTY(VisibleInstanceOnly, Category = "GameInfo")
    float ElapsedTime;
    UPROPERTY(VisibleInstanceOnly, Category = "GameInfo")
    int32 SpawnLimit;
    UPROPERTY(VisibleInstanceOnly, Category = "GameInfo")
    class ADEMonsterSpawnManager* MonsterSpawnManager = nullptr;




    // ... 나머지 매니저 포인터도 선언
  
};
