// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPlayerController.h"
#include "DEGameMode_Stage.h"
#include "DEGameHUDWidget.h"
#include "DELevelUpWidget.h"
#include "DECharacterBase.h"
#include "DESkillManagerComponent.h"
#include "DESkillInventoryWidget.h"
#include "Kismet/GameplayStatics.h"

ADEPlayerController::ADEPlayerController()
{
    static ConstructorHelpers::FClassFinder<UDEGameHUDWidget> UI_GameHUDWidget_C(TEXT("/Game/DarkEden/UI/UI_GameHUD.UI_GameHUD_C"));
    if (UI_GameHUDWidget_C.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("UI CLASS Succeeded"));
        GameHUDWidgetClass = UI_GameHUDWidget_C.Class;

    }
    static ConstructorHelpers::FClassFinder<UDESkillInventoryWidget> UI_SkillInventoryWidget_C(TEXT("/Game/DarkEden/UI/WBP_SkillInventoryWidget.WBP_SkillInventoryWidget_C"));
    if (UI_SkillInventoryWidget_C.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("Inventory UI Succeeded"));
        SkillInventoryWidgetClass = UI_SkillInventoryWidget_C.Class;

    }
    static ConstructorHelpers::FClassFinder<UDELevelUpWidget> UI_LevelUpWidget_C(TEXT("/Game/DarkEden/UI/UI_SelectSkill.UI_SelectSkill_C"));
    if (UI_LevelUpWidget_C.Succeeded())
    {
        LevelUpWidgetClass = UI_LevelUpWidget_C.Class;

    }
}

void ADEPlayerController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("Try Show Time UI"));
    if (GameHUDWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("There IS TIME UI"));
        GameHUDWidget = CreateWidget<UDEGameHUDWidget>(this, GameHUDWidgetClass);
        if (GameHUDWidget)
        {
            GameHUDWidget->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("Time UI Succeeded"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("NO TIME UI"));
    }
    if (SkillInventoryWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("There IS Inventory UI"));
        UDESkillInventoryWidget* Widget =
            CreateWidget<UDESkillInventoryWidget>(this, SkillInventoryWidgetClass);

        Widget->AddToViewport();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("NO Inventory UI"));
    }
    
}

void ADEPlayerController::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void ADEPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);
}

void ADEPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    Super::Tick(DeltaSeconds);

    if (GameHUDWidget)
    {
        float Time = GetWorld()->GetAuthGameMode<ADEGameMode_Stage>()->GetElapsedTime();
        GameHUDWidget->UpdateTime(Time);
    }


}

void ADEPlayerController::ShowLevelUpUI()
{
    UE_LOG(LogTemp, Warning, TEXT("Try Show Level Up UI"));
    APawn* MyPawn = GetPawn();
    if (!MyPawn) return;
    UE_LOG(LogTemp, Warning, TEXT("GetPawn Passed"));
    ADECharacterBase* MyChar = Cast<ADECharacterBase>(MyPawn);
    UDESkillManagerComponent* SkillManager = MyChar->GetSkillManagerComponent();
    if (!MyChar || !SkillManager) return;
    UE_LOG(LogTemp, Warning, TEXT("MyChar, SkillManager Passed"));
    if (!LevelUpWidgetClass || !SkillManager) return;
    UE_LOG(LogTemp, Warning, TEXT("WidgetClass Passed"));
    UDELevelUpWidget* Widget = CreateWidget<UDELevelUpWidget>(this, LevelUpWidgetClass);
    if (!Widget) return;
    UE_LOG(LogTemp, Warning, TEXT("Created Widget"));
    Widget->InitWidget(SkillManager);
    Widget->AddToViewport();

    SetPause(true);

    FInputModeUIOnly InputMode;
    SetInputMode(InputMode);
    bShowMouseCursor = true;
}
void ADEPlayerController::ResumeGame()
{
    SetPause(false);
    FInputModeGameOnly Mode;
    SetInputMode(Mode);
    bShowMouseCursor = false;
}