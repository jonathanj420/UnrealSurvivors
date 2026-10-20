// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameMode_Stage.h"
#include "DEFemaleVampire.h"
#include "DEPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "DEMonsterSpawnManager.h"
#include "DEResultWidget.h" // 아까 만든 위젯 헤더
#include "DEGameInstance.h"
#include "DECombatComponent.h"
#include "DECharacterRow.h"
#include "DECharacterBase.h"
#include "DEStatComponent.h"
#include "DEStatTypes.h"
#include "DarkEdenSurvivors.h"

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

    if (bStageEnded) return;

    ElapsedTime += DeltaTime;

    if (StageDuration > 0.0f && ElapsedTime >= StageDuration)
    {
        ElapsedTime = StageDuration;
        StageClear();
    }
}

float ADEGameMode_Stage::GetMonsterHPMultiplier() const
{
    return EvaluateDifficultyCurve(MonsterHPCurve) * GetCurseMultiplier();
}

float ADEGameMode_Stage::GetMonsterDamageMultiplier() const
{
    return EvaluateDifficultyCurve(MonsterDamageCurve) * GetCurseMultiplier();
}

float ADEGameMode_Stage::GetCurseMultiplier() const
{
    // Curse 스탯은 기본 1 (저주 없음). 멀티에서는 파티 중 가장 높은 값을 쓴다
    float Curse = 1.0f;
    bool bFound = false;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        const ADECharacterBase* Player = It->IsValid() ? Cast<ADECharacterBase>((*It)->GetPawn()) : nullptr;
        const UDEStatComponent* Stats = Player ? Player->GetStatComponent() : nullptr;
        if (!Stats) continue;

        const float PlayerCurse = Stats->GetStatValue(EDEStatType::Curse);
        Curse = bFound ? FMath::Max(Curse, PlayerCurse) : PlayerCurse;
        bFound = true;
    }

    return FMath::Max(Curse, 0.01f);
}

float ADEGameMode_Stage::EvaluateDifficultyCurve(const FRuntimeFloatCurve& Curve) const
{
    const FRichCurve* RichCurve = Curve.GetRichCurveConst();
    if (!RichCurve || RichCurve->GetNumKeys() == 0)
    {
        return 1.0f;
    }

    // 0 이하 배율은 HP 0 몬스터를 만들므로 막는다
    return FMath::Max(RichCurve->Eval(ElapsedTime / 60.0f, 1.0f), 0.01f);
}

void ADEGameMode_Stage::BeginPlay()
{
    Super::BeginPlay();
}

FString ADEGameMode_Stage::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
    const FString ErrorMessage = Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);

    if (ADEPlayerController* PC = Cast<ADEPlayerController>(NewPlayerController))
    {
        FString CharacterOption = UGameplayStatics::ParseOption(Options, TEXT("Character"));

        // 호스트(로컬) 플레이어는 맵 URL 옵션이 곧 자기 선택이다. 접속 옵션에 안 실려 왔을 때를 대비해 맵 옵션도 본다
        if (CharacterOption.IsEmpty() && PC->IsLocalController())
        {
            CharacterOption = UGameplayStatics::ParseOption(OptionsString, TEXT("Character"));
        }

        if (!CharacterOption.IsEmpty())
        {
            PC->SetSelectedCharacterID(FCString::Atoi(*CharacterOption));
        }
    }

    return ErrorMessage;
}

UClass* ADEGameMode_Stage::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    const ADEPlayerController* PC = Cast<ADEPlayerController>(InController);
    const UDEGameInstance* GI = GetGameInstance<UDEGameInstance>();

    if (PC && GI && PC->GetSelectedCharacterID() != INDEX_NONE)
    {
        const FDECharacterRow* Row = GI->GetCharacterRow(PC->GetSelectedCharacterID());
        if (Row && Row->CharacterClass)
        {
            return Row->CharacterClass;
        }

        UE_LOG(LogDarkEdenSurvivors, Warning, TEXT("[Stage] Character %d has no row or CharacterClass, using DefaultPawnClass"), PC->GetSelectedCharacterID());
    }

    return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ADEGameMode_Stage::SetPlayerDefaults(APawn* PlayerPawn)
{
    Super::SetPlayerDefaults(PlayerPawn);

    if (ADECharacterBase* Player = Cast<ADECharacterBase>(PlayerPawn))
    {
        // 같은 폰으로 다시 불려도 중복 바인딩되지 않게
        Player->OnPlayerDied.RemoveAll(this);
        Player->OnPlayerDied.AddUObject(this, &ADEGameMode_Stage::GameOver);
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
    EndStage(false);
}

void ADEGameMode_Stage::StageClear()
{
    EndStage(true);
}

void ADEGameMode_Stage::EndStage(bool bCleared)
{
    // 사망과 클리어가 같은 프레임에 겹치거나 나가기가 두 번 눌려도 정산은 한 번만
    if (bStageEnded) return;
    bStageEnded = true;

    UE_LOG(LogDarkEdenSurvivors, Log, TEXT("[Stage] %s at %.1fs"), bCleared ? TEXT("Cleared") : TEXT("Game over"), ElapsedTime);

    // 1. 게임 일시 정지. 일시정지와 입력 모드는 각 플레이어 컨트롤러가 관리한다 (클리어도 같은 이유를 쓴다)
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (ADEPlayerController* PC = Cast<ADEPlayerController>(It->Get()))
        {
            PC->AddPauseReason(EDEPauseReason::GameOver);
        }
    }

    // 골드 정산과 저장은 UI와 무관하게 GameMode에서 끝낸다
    if (UDEGameInstance* GI = Cast<UDEGameInstance>(GetGameInstance()))
    {
        GI->AddGold(EarnedGold);
        GI->SaveGame();
    }

    // 2. 위젯 생성
    if (ResultWidgetClass)
    {
        UDEResultWidget* ResultUI = CreateWidget<UDEResultWidget>(GetWorld(), ResultWidgetClass);
        if (ResultUI)
        {
            ResultUI->AddToViewport(9999); // 최상단 노출

            // 3. 데이터 수집 (예시 값)
            int32 FinalGold = EarnedGold;
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

            // 4. 위젯에 데이터 전달
            ResultUI->SetResultData(FinalGold, FinalKills, FinalDamageDealt, FinalTime, bCleared);
        }
    }

}

void ADEGameMode_Stage::AddGold(int32 Amount)
{
    if (Amount <= 0) return;

    EarnedGold += Amount;

    // (보너스) 나중에 화면 우상단에 "현재 골드: 150" 이런 UI가 있다면 여기서 갱신해주면 됨!
}