// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameMode.h"
#include "TestCharacter.h"
#include "DEFemaleVampire.h"
#include "DEPlayerController.h"
#include "Kismet/GameplayStatics.h"

ADEGameMode::ADEGameMode()
{
    DefaultPawnClass = ADEFemaleVampire::StaticClass();
    PlayerControllerClass = ADEPlayerController::StaticClass();

    //PlayerControllerClass = AABPlayerController::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
	ElapsedTime = 0.0f;
    SpawnLimit = 300;

}

void ADEGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;
}

void ADEGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    UWorld* World = GetWorld();
    if (World)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this; // GameMode를 소유자로 지정
        // ... 나머지 SpawnerManager와 PlayerSystemManager도 동일하게 생성합니다.
    }

}
