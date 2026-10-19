// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPlayerController.h"
#include "DEGameMode_Stage.h"
#include "DEGameHUDWidget.h"
#include "DELevelUpWidget.h"
#include "DECharacterBase.h"
#include "DELevelUpManagerComponent.h"
#include "DESkillInventoryWidget.h"
#include "Kismet/GameplayStatics.h"
#include "DEChestWidget.h"
#include "DELevelUpChoiceBase.h"
#include "DarkEdenSurvivors.h"

ADEPlayerController::ADEPlayerController()
{
    static ConstructorHelpers::FClassFinder<UDEGameHUDWidget> UI_GameHUDWidget_C(TEXT("/Game/DarkEden/UI/UI_GameHUD.UI_GameHUD_C"));
    if (UI_GameHUDWidget_C.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("Gmae HUD Succeeded"));
        GameHUDWidgetClass = UI_GameHUDWidget_C.Class;

    }
    
    static ConstructorHelpers::FClassFinder<UDELevelUpWidget> UI_LevelUpWidget_C(TEXT("/Game/DarkEden/UI/WBP_LevelUpWidget.WBP_LevelUpWidget_C"));
    if (UI_LevelUpWidget_C.Succeeded())
    {
        LevelUpWidgetClass = UI_LevelUpWidget_C.Class;
        UE_LOG(LogTemp, Warning, TEXT("Level Up Widget Succeeded"));

    }

    static ConstructorHelpers::FClassFinder<UDEChestWidget> UI_ChestWidget_C(TEXT("/Game/DarkEden/UI/WBP_ChestWidget.WBP_ChestWidget_C"));
    if (UI_ChestWidget_C.Succeeded())
    {
        UE_LOG(LogTemp, Warning, TEXT("Inventory UI Succeeded"));
        ChestWidgetClass = UI_ChestWidget_C.Class;

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

    if (UDELevelUpManagerComponent* LevelUpManager = GetPawnLevelUpManager())
    {
        LevelUpManager->OnLevelUpChoicesReady.AddUObject(this, &ADEPlayerController::ShowLevelUpChoices);
        LevelUpManager->OnLevelUpClosed.AddUObject(this, &ADEPlayerController::HandleLevelUpClosed);
        LevelUpManager->OnChestRewardsReady.AddUObject(this, &ADEPlayerController::ShowChestWidget);
        LevelUpManager->OnChestClosed.AddUObject(this, &ADEPlayerController::HandleChestClosed);
    }
}

void ADEPlayerController::OnUnPossess()
{
    if (UDELevelUpManagerComponent* LevelUpManager = GetPawnLevelUpManager())
    {
        LevelUpManager->OnLevelUpChoicesReady.RemoveAll(this);
        LevelUpManager->OnLevelUpClosed.RemoveAll(this);
        LevelUpManager->OnChestRewardsReady.RemoveAll(this);
        LevelUpManager->OnChestClosed.RemoveAll(this);
    }

    Super::OnUnPossess();
}

UDELevelUpManagerComponent* ADEPlayerController::GetPawnLevelUpManager() const
{
    const ADECharacterBase* MyChar = Cast<ADECharacterBase>(GetPawn());
    return MyChar ? MyChar->GetLevelUpManagerComponent() : nullptr;
}

void ADEPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);


}

void ADEPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // 입력 컴포넌트가 제대로 있는지 확인
    if (InputComponent)
    {
        // "PauseGame" 액션을 눌렀을 때(IE_Pressed), 내 컨트롤러의 TogglePauseMenu 함수를 실행해라!
        FInputActionBinding& PauseBinding = InputComponent->BindAction(
            TEXT("PauseGame"),
            IE_Pressed,
            this,
            &ADEPlayerController::TogglePauseMenu
        );

        // =========================================================
        // ★ [핵심] 이거 안 쓰면 일시정지 후 ESC 키가 먹통이 됨!!!
        // =========================================================
        PauseBinding.bExecuteWhenPaused = true;
    }

}

void ADEPlayerController::ShowLevelUpChoices(const TArray<UDELevelUpChoiceBase*>& Choices)
{
    UDELevelUpManagerComponent* LevelUpManager = GetPawnLevelUpManager();
    if (!LevelUpManager) return;

    if (!LevelUpWidget)
    {
        if (!LevelUpWidgetClass)
        {
            UE_LOG(LogDarkEdenSurvivors, Error, TEXT("[LevelUp] LevelUpWidgetClass is not set"));
            return;
        }
        LevelUpWidget = CreateWidget<UDELevelUpWidget>(this, LevelUpWidgetClass);
        if (!LevelUpWidget) return;
    }

    LevelUpWidget->InitWidget(LevelUpManager);
    if (!LevelUpWidget->IsInViewport())
    {
        LevelUpWidget->AddToViewport();
    }
    LevelUpWidget->SetChoices(Choices);

    AddPauseReason(EDEPauseReason::LevelUp);
}

void ADEPlayerController::HandleLevelUpClosed()
{
    if (LevelUpWidget)
    {
        LevelUpWidget->RemoveFromParent();
    }
    RemovePauseReason(EDEPauseReason::LevelUp);
}

void ADEPlayerController::HandleChestClosed()
{
    if (ChestWidget)
    {
        ChestWidget->RemoveFromParent();
    }
    RemovePauseReason(EDEPauseReason::Chest);
}

void ADEPlayerController::AddPauseReason(EDEPauseReason Reason)
{
    PauseReasons.Add(Reason);
    ApplyPauseState();
}

void ADEPlayerController::RemovePauseReason(EDEPauseReason Reason)
{
    PauseReasons.Remove(Reason);
    ApplyPauseState();
}

void ADEPlayerController::ApplyPauseState()
{
    // 멀티플레이에서는 SetPause가 모든 플레이어를 멈추므로 이유별로 월드를 멈출지 다시 정해야 한다. 그 판단은 여기 한 곳에서 한다
    const bool bShouldPause = !PauseReasons.IsEmpty();
    SetPause(bShouldPause);
    bShowMouseCursor = bShouldPause;

    if (bShouldPause)
    {
        // UIOnly로 하면 ESC 바인딩(bExecuteWhenPaused)까지 막히므로 GameAndUI를 쓴다. 월드가 멈춰 있어 이동 입력은 먹지 않는다
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        SetInputMode(InputMode);
    }
    else
    {
        SetInputMode(FInputModeGameOnly());
    }
}

void ADEPlayerController::ShowChestWidget(const TArray<UDELevelUpChoiceBase*>& Rewards)
{
    UDELevelUpManagerComponent* LevelUpManager = GetPawnLevelUpManager();
    if (!LevelUpManager) return;

    // 1. 위젯 클래스 할당 여부 체크 (방어 코드)
    if (!ChestWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("No Chest Widget"));
        return;
    }

    // 2. 위젯 인스턴스가 없으면 동적 생성
    if (!ChestWidget)
    {
        ChestWidget = CreateWidget<UDEChestWidget>(this, ChestWidgetClass);
    }

    if (ChestWidget)
    {
        // 3. 화면에 없으면 띄우기
        if (!ChestWidget->IsInViewport())
        {
            ChestWidget->AddToViewport();
        }

        ChestWidget->InitWidget(LevelUpManager);
        ChestWidget->SetupChestRewards(Rewards);

        AddPauseReason(EDEPauseReason::Chest);
    }
}

void ADEPlayerController::TogglePauseMenu()
{
    if (PauseReasons.Contains(EDEPauseReason::PauseMenu))
    {
        ClosePauseMenu();
        return;
    }

    // 멈춘 동안 입력 모드가 GameAndUI라 레벨업/상자/게임오버 창이 떠 있어도 ESC가 들어온다. 그때는 열지 않는다
    if (!PauseReasons.IsEmpty() || !PauseMenuClass)
    {
        return;
    }

    if (!PauseMenuInstance)
    {
        PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);
    }

    if (PauseMenuInstance)
    {
        PauseMenuInstance->AddToViewport(100); // ZOrder 높게 줘서 맨 위에 뜨게
        AddPauseReason(EDEPauseReason::PauseMenu);
    }
}

void ADEPlayerController::ClosePauseMenu()
{
    if (PauseMenuInstance)
    {
        PauseMenuInstance->RemoveFromParent();
    }
    RemovePauseReason(EDEPauseReason::PauseMenu);
}
