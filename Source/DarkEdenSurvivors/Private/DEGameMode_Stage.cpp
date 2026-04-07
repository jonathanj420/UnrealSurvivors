// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameMode_Stage.h"
#include "DEFemaleVampire.h"
#include "DEPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DEMonsterSpawnManager.h"
#include "DEResultWidget.h" // 아까 만든 위젯 헤더
#include "DEGameInstance.h"
#include "DECombatComponent.h"

ADEGameMode_Stage::ADEGameMode_Stage()
{
    PrimaryActorTick.bCanEverTick = true;
    ElapsedTime = 0.0f;
    SpawnLimit = 300;

    DefaultPawnClass = ADEFemaleVampire::StaticClass();
    PlayerControllerClass = ADEPlayerController::StaticClass();

    static ConstructorHelpers::FClassFinder<UDEResultWidget> WBP_ResultWidget(
        TEXT("/Game/DarkEden/UI/WBP_Result.WBP_Result_C")
    );

    if (WBP_ResultWidget.Succeeded())
    {
        ResultWidgetClass = WBP_ResultWidget.Class;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load WBP_Result! Check path."));
    }

    

}

void ADEGameMode_Stage::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ElapsedTime += DeltaTime;
}

void ADEGameMode_Stage::BeginPlay()
{
    Super::BeginPlay();

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (ADECharacterBase* Player = Cast<ADECharacterBase>(PlayerPawn))
    {
        // [수정] AddDynamic -> AddUObject
        // 이제 함수 이름(String)이 아니라 함수 주소(&)를 직접 연결합니다. 훨씬 빠릅니다.
        Player->OnPlayerDied.AddUObject(this, &ADEGameMode_Stage::GameOver);
    }

    UWorld* World = GetWorld();
    if (World)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this; // GameMode를 소유자로 지정
        // ... 나머지 SpawnerManager와 PlayerSystemManager도 동일하게 생성합니다.
    }

}

void ADEGameMode_Stage::RegisterMonsterSpawnManager(ADEMonsterSpawnManager* InManager)
{
    UE_LOG(LogTemp, Warning, TEXT("Try Spawn Manager Register"));
    if (InManager != nullptr)
    {
        MonsterSpawnManager = InManager;
        UE_LOG(LogTemp, Warning, TEXT("Monster Manager Registered"));
    }

}

const TArray<class ADEMonsterBase*>& ADEGameMode_Stage::GetActiveMonsters() const
{
    return MonsterSpawnManager->GetActiveMonsters();
}

void ADEGameMode_Stage::GameOver()
{
    UE_LOG(LogTemp, Warning, TEXT("=== GAME OVER ==="));

    // 1. 게임 일시 정지 (선택 사항: 몬스터 멈추게 하려면 true)
     UGameplayStatics::SetGamePaused(this, true);

    // 2. 위젯 생성
    if (ResultWidgetClass)
    {
        UDEResultWidget* ResultUI = CreateWidget<UDEResultWidget>(GetWorld(), ResultWidgetClass);
        if (ResultUI)
        {
            ResultUI->AddToViewport(9999); // 최상단 노출

            // 3. 데이터 수집 (예시 값)
            int32 FinalGold = 0;
            int32 FinalKills = 0;
            float FinalDamageDealt = 0.0f;
            float FinalTime = GetElapsedTime(); // GameMode에 있는 시간

            if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
            {
                // 플레이어가 조종 중인 캐릭터(Pawn)를 가져옴
                if (APawn* PlayerPawn = PC->GetPawn())
                {
                    // 캐릭터 몸에 붙어있는 CombatComponent를 찾음!
                    if (UDECombatComponent* CombatComp = PlayerPawn->FindComponentByClass<UDECombatComponent>())
                    {
                        // 컴포넌트에서 킬 수 가져오기 (변수가 public이 아니라면 Getter 함수를 써줘!)
                        FinalKills = CombatComp->GetTotalKillCount();
                        FinalDamageDealt = CombatComp->GetTotalDamageDealt();
                    }
                }
            }

            // ========================================================
            // ★ [골드 정산 로직] 
            // ========================================================
            FinalGold = EarnedGold; // 1. 이번 판 장부에서 돈을 확인한다.

            // 2. 은행(GameInstance)에 가서 이번 판에 번 돈을 저금한다!
            if (UDEGameInstance* GI = Cast<UDEGameInstance>(GetGameInstance()))
            {
                // (GI 쪽에 AddTotalGold 같은 누적 함수가 있다고 가정)
                GI->AddGold(FinalGold);

                // UE_LOG(LogTemp, Warning, TEXT("전 재산: %d"), GI->GetTotalGold());
            }
            //// GameInstance에서 실제 골드 가져오기 (혹은 이번 판 번 돈)
            //if (UDEGameInstance* GI = Cast<UDEGameInstance>(GetGameInstance()))
            //{
            //    FinalGold = GI->GetTotalGold();
            //    // 참고: 이번 판에 번 돈만 보여주고 싶다면 GameMode에 'EarnedGold' 변수를 따로 둬야 함
            //}

            // 4. 위젯에 데이터 전달
            ResultUI->SetResultData(FinalGold, FinalKills, FinalDamageDealt, FinalTime);

            // 5. 마우스 커서 보이게 설정 (중요! 안 하면 버튼 못 누름)
            if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
            {
                PC->bShowMouseCursor = true;
                PC->SetInputMode(FInputModeUIOnly()); // 게임 조작 막고 UI만
            }
        }
    }

}

void ADEGameMode_Stage::AddGold(int32 Amount)
{
    if (Amount <= 0) return;

    EarnedGold += Amount;

    // (보너스) 나중에 화면 우상단에 "현재 골드: 150" 이런 UI가 있다면 여기서 갱신해주면 됨!
}