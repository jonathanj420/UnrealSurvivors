// Fill out your copyright notice in the Description page of Project Settings.


#include "DEMonsterRepositionManager.h"
#include "DEGameMode.h"
#include "DEMonsterSpawnManager.h"
#include "DEMonsterBase.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
// Sets default values
ADEMonsterRepositionManager::ADEMonsterRepositionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ADEMonsterRepositionManager::BeginPlay()
{
    Super::BeginPlay();

    Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    GameMode=Player->GetWorld()->GetAuthGameMode<ADEGameMode>();
    // 1. 게임모드한테 먼저 물어봄
    if (GameMode)
    {
        MonsterSpawnManager = GameMode->GetMonsterSpawnManager();
        UE_LOG(LogTemp, Warning, TEXT("Monster Manager Found Successfully"));
    }

    // 2. [핵심] 만약 게임모드가 "모르는데?(nullptr)"라고 하면, 직접 월드를 뒤져서 찾음
    if (!MonsterSpawnManager)
    {
        AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADEMonsterSpawnManager::StaticClass());
        MonsterSpawnManager = Cast<ADEMonsterSpawnManager>(FoundActor);
        UE_LOG(LogTemp, Warning, TEXT("Monster Manager Found Forced"));
    }

    // 3. 그래도 없으면 진짜 문제임. (크래시 대신 로그 띄우고 리턴)
    if (!MonsterSpawnManager)
    {
        UE_LOG(LogTemp, Error, TEXT("RepositionManager: CRITICAL ERROR - Cannot find MonsterSpawnManager!"));
        return;
    }

    check(Player);
    check(MonsterSpawnManager);
}

void ADEMonsterRepositionManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    const TArray<ADEMonsterBase*>& Monsters =
        MonsterSpawnManager->GetActiveMonsters();

    const FVector PlayerLoc = Player->GetActorLocation();
    const float MaxDistSq = MaxDistanceFromPlayer * MaxDistanceFromPlayer;


    for (ADEMonsterBase* Monster : Monsters)
    {
        if (!Monster->IsAlive())
            continue;

        const float DistSq =
            FVector::DistSquared(Monster->GetActorLocation(), PlayerLoc);

        if (DistSq > MaxDistSq)
        {
            RepositionMonster(Monster);
        }
    }
}

void ADEMonsterRepositionManager::RepositionMonster(ADEMonsterBase* Monster)
{
    if (!Monster) return;

    const FVector NewPos = GetRandomPositionAroundPlayer();

    // 순간이동
    Monster->SetActorLocation(NewPos, false, nullptr, ETeleportType::TeleportPhysics);

    // 타겟 갱신 (중요)
    //Monster->OnRepositioned();
}

FVector ADEMonsterRepositionManager::GetRandomPositionAroundPlayer() const
{
    const FVector PlayerLoc = Player->GetActorLocation();

    // 랜덤 방향 (원형)
    const float Angle = FMath::FRandRange(0.f, 2.f * PI);
    const float Radius = FMath::FRandRange(RespawnMinRadius, RespawnMaxRadius);

    FVector Offset;
    Offset.X = FMath::Cos(Angle) * Radius;
    Offset.Y = FMath::Sin(Angle) * Radius;
    Offset.Z = 0.f;

    return PlayerLoc + Offset;
}
