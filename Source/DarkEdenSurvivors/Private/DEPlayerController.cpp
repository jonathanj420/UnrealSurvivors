// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPlayerController.h"
#include "DEGameMode_Stage.h"
#include "DEGameHUDWidget.h"
#include "DELevelUpWidget.h"
#include "DECharacterBase.h"
#include "DELevelUpManagerComponent.h"
#include "DESkillInventoryWidget.h"
#include "Kismet/GameplayStatics.h"

ADEPlayerController::ADEPlayerController()
{
    static ConstructorHelpers::FClassFinder<UDEGameHUDWidget> UI_GameHUDWidget_C(TEXT("/Game/DarkEden/UI/UI_GameHUD.UI_GameHUD_C"));
    if (UI_GameHUDWidget_C.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("Gmae HUD Succeeded"));
        GameHUDWidgetClass = UI_GameHUDWidget_C.Class;

    }
    //static ConstructorHelpers::FClassFinder<UDESkillInventoryWidget> UI_SkillInventoryWidget_C(TEXT("/Game/DarkEden/UI/WBP_SkillInventoryWidget.WBP_SkillInventoryWidget_C"));
    //if (UI_SkillInventoryWidget_C.Succeeded())
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("Inventory UI Succeeded"));
    //    SkillInventoryWidgetClass = UI_SkillInventoryWidget_C.Class;

    //}
    static ConstructorHelpers::FClassFinder<UDELevelUpWidget> UI_LevelUpWidget_C(TEXT("/Game/DarkEden/UI/WBP_LevelUpWidget.WBP_LevelUpWidget_C"));
    if (UI_LevelUpWidget_C.Succeeded())
    {
        LevelUpWidgetClass = UI_LevelUpWidget_C.Class;

    }
}

void ADEPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;

    // 1. 현재 게임 모드가 "스테이지"인지 확인 (로비에서는 안 띄우려고)
    // (만약 로비용 컨트롤러를 따로 안 만들고 하나로 쓴다면 이 체크가 필수입니다)
    if (GetWorld()->GetAuthGameMode<ADEGameMode_Stage>())
    {
        if (GameHUDWidgetClass)
        {
            GameHUDWidget = CreateWidget<UDEGameHUDWidget>(this, GameHUDWidgetClass);
            if (GameHUDWidget)
            {
                GameHUDWidget->AddToViewport();
                if (ADECharacterBase* MyChar = Cast<ADECharacterBase>(GetPawn()))
                {
                    GameHUDWidget->BindCharacterStat(MyChar);
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("NO GAME HUD"));
        }

        // 3. 입력 모드 게임으로 전환 (HUD 띄웠으니까)
        FInputModeGameOnly GameInputMode;
        SetInputMode(GameInputMode);
        bShowMouseCursor = false;
    }

    
    //if (SkillInventoryWidgetClass)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("There IS Inventory UI"));
    //    UDESkillInventoryWidget* Widget =
    //        CreateWidget<UDESkillInventoryWidget>(this, SkillInventoryWidgetClass);

    //    Widget->AddToViewport();
    //}
    //else
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("NO Inventory UI"));
    //}
    
}

void ADEPlayerController::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void ADEPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);
    // 새로 들어온 폰이 우리 캐릭터고, HUD가 켜져 있다면
    ADECharacterBase* MyChar = Cast<ADECharacterBase>(aPawn);
    if (MyChar && GameHUDWidget)
    {
        // HUD한테 새 캐릭터 스탯 연결하라고 명령
        GameHUDWidget->BindCharacterStat(MyChar);
    }

}

void ADEPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    //Super::Tick(DeltaSeconds);

    //if (GameHUDWidget)
    //{
    //    float Time = GetWorld()->GetAuthGameMode<ADEGameMode_Stage>()->GetElapsedTime();
    //    GameHUDWidget->UpdateTime(Time);
    //}


}

void ADEPlayerController::ShowLevelUpUI()
{
    UE_LOG(LogTemp, Warning, TEXT("Try Show Level Up UI"));
    APawn* MyPawn = GetPawn();
    if (!MyPawn) return;
    UE_LOG(LogTemp, Warning, TEXT("GetPawn Passed"));
    ADECharacterBase* MyChar = Cast<ADECharacterBase>(MyPawn);
    UDELevelUpManagerComponent* LevelUpManager = MyChar->GetLevelUpManagerComponent();
    if (!MyChar || !LevelUpManager) return;
    UE_LOG(LogTemp, Warning, TEXT("MyChar, SkillManager Passed"));
    if (!LevelUpWidgetClass || !LevelUpManager) return;
    UE_LOG(LogTemp, Warning, TEXT("WidgetClass Passed"));
    UDELevelUpWidget* Widget = CreateWidget<UDELevelUpWidget>(this, LevelUpWidgetClass);
    if (!Widget) return;
    UE_LOG(LogTemp, Warning, TEXT("Created Widget"));
    Widget->InitWidget(LevelUpManager);
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