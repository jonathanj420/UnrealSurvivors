// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameMode_Lobby.h"
#include "Kismet/GameplayStatics.h"
#include "DEPlayerController_Lobby.h"
#include "DEGameInstance.h"

ADEGameMode_Lobby::ADEGameMode_Lobby()
{
    PrimaryActorTick.bCanEverTick = false;

    DefaultPawnClass = nullptr;
    PlayerControllerClass = ADEPlayerController_Lobby::StaticClass();


}

void ADEGameMode_Lobby::BeginPlay()
{
    Super::BeginPlay();

    //if (MainMenuWidgetClass)
    //{
    //    UUserWidget* Widget = CreateWidget<UUserWidget>(
    //        GetWorld(), MainMenuWidgetClass);
    //    if (Widget)
    //        Widget->AddToViewport();
    //}
}