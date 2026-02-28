// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPlayerController_Lobby.h"
#include "DEMainMenuWidget.h"


ADEPlayerController_Lobby::ADEPlayerController_Lobby()
{
    ConstructorHelpers::FClassFinder<UDEMainMenuWidget> WBP_MainMenuWidget_C(TEXT("/Game/DarkEden/UI/WBP_MainMenuWidget.WBP_MainMenuWidget_C"));
    if (WBP_MainMenuWidget_C.Succeeded())
    {
        MainMenuWidgetClass = WBP_MainMenuWidget_C.Class;
    }


}
void ADEPlayerController_Lobby::BeginPlay()
{
    Super::BeginPlay();
    bShowMouseCursor = true;
    SetInputMode(FInputModeUIOnly());


    if (MainMenuWidgetClass)
    {
        MainMenuWidget = CreateWidget<UDEMainMenuWidget>(this, MainMenuWidgetClass);
        if (MainMenuWidget)
            MainMenuWidget->AddToViewport();
    }

}