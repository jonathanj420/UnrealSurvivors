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
protected:
    UPROPERTY(VisibleInstanceOnly, Category = "Time")
    float ElapsedTime;


    // ... 나머지 매니저 포인터도 선언
  
};
