// Fill out your copyright notice in the Description page of Project Settings.


#include "DEMonsterSpawnManager.h"
#include "DEMonsterBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DECharacterBase.h"
#include "DEGameMode.h"

ADEMonsterSpawnManager::ADEMonsterSpawnManager()
{
    PrimaryActorTick.bCanEverTick = true;
    //MonsterClass = ADEMonsterBase::StaticClass();
    MonsterClasses.Add(ADEMonsterBase::StaticClass());
    //MonsterClasses.Add(ADEMonsterBase::StaticClass());
    //MonsterClasses.Add(ADEMonsterBase::StaticClass()); . . . . . 

    CurrentWaveIndex = -1;
    WaveElapsedTime = 0.0f;
    NextSpawnTime = 0.0f;


    USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootSceneComponent;
}

void ADEMonsterSpawnManager::BeginPlay()
{
    Super::BeginPlay();
    GameMode = Cast<ADEGameMode>(UGameplayStatics::GetGameMode(this));
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PlayerController)
    {
        APawn* PlayerPawn = PlayerController->GetPawn();

        Player = Cast<ADECharacterBase>(PlayerPawn);

    }
    // StageWaveTable이 있으면 RowName을 읽어와 StartTime 기준으로 정렬해서 캐시함
    StageRowNames.Empty();
    if (StageWaveTable)
    {
        struct FRowPair { FName Name; float Start; };
        TArray<FRowPair> Pairs;

        TArray<FName> AllNames = StageWaveTable->GetRowNames();
        for (FName RN : AllNames)
        {
            const FDEStageWaveData* Row = StageWaveTable->FindRow<FDEStageWaveData>(RN, TEXT(""));
            if (Row)
            {
                Pairs.Add({ RN, Row->StartTime });
            }
        }

        // StartTime 오름차순 정렬
        Pairs.Sort([](const FRowPair& A, const FRowPair& B) {
            return A.Start < B.Start;
            });

        for (const FRowPair& P : Pairs)
            StageRowNames.Add(P.Name);

        UE_LOG(LogTemp, Warning, TEXT("StageWaveTable loaded: %d rows"), StageRowNames.Num());
    }

    // 초기화
    CurrentWaveIndex = -1;
    WaveElapsedTime = 0.0f;
    NextSpawnTime = 0.0f;
    /*InitializePool();

    StartNextWave();*/

}

void ADEMonsterSpawnManager::Tick(float DeltaTime)
{
    //control all monsters' logic here
    Super::Tick(DeltaTime);

    if (!Player) return;

    ProcessWave(DeltaTime);

    FVector PlayerLocation = Player->GetActorLocation();
    for (int32 i = 0; i < ActiveMonsters.Num(); ++i)
    {
        ADEMonsterBase* Mob = ActiveMonsters[i];
        if (!Mob) continue;

        Mob->UpdateKnockback(DeltaTime);
        Mob->MoveToPlayer(DeltaTime);
    }

    // 2) push-out (겹침 해소) - O(n^2), 필요시 spatial partitioning 사용
    for (int32 i = ActiveMonsters.Num() - 1; i >= 0; --i)
    {
        ADEMonsterBase* A = ActiveMonsters[i];
        if (!A) continue;

        for (int32 j = i - 1; j >= 0; --j)
        {
            ADEMonsterBase* B = ActiveMonsters[j];
            if (!B) continue;

            ResolveMonsterOverlap(A, B);
        }
    }

    /*ActiveMonsters.RemoveAllSwap([&](ADEMonsterBase* Mob)
        {
            return (!Mob || Mob->IsDead());
        });*/

}

//************************NEW FUNC
void ADEMonsterSpawnManager::ProcessWave(float DeltaTime)
{
    if (!GameMode || StageRowNames.Num() == 0)
        return;

    const float Elapsed = GameMode->GetElapsedTime();

    // 다음 웨이브가 준비되었는지 확인 (현재 인덱스 +1)
    int32 NextIndex = CurrentWaveIndex + 1;
    if (NextIndex < StageRowNames.Num())
    {
        FName NextName = StageRowNames[NextIndex];
        const FDEStageWaveData* NextRow = StageWaveTable->FindRow<FDEStageWaveData>(NextName, TEXT(""));
        if (NextRow && Elapsed >= NextRow->StartTime)
        {
            CurrentWaveIndex = NextIndex;
            StartWave(CurrentWaveIndex);
        }
    }

    // 현재 활성 웨이브가 없다면 종료
    if (CurrentWaveIndex < 0 || CurrentWaveIndex >= StageRowNames.Num())
        return;

    // 현재 웨이브 데이터 가져오기
    FName CurName = StageRowNames[CurrentWaveIndex];
    const FDEStageWaveData* CurRow = StageWaveTable->FindRow<FDEStageWaveData>(CurName, TEXT(""));
    if (!CurRow) return;

    // 누적 시간 업데이트
    WaveElapsedTime += DeltaTime;

    // Duration이 0이면 무한(또는 단발 보스만)으로 간주 — Duration > 0 이면 그 기간까지만 Interval 스폰
    if (CurRow->Duration > 0.0f && WaveElapsedTime >= CurRow->Duration)
    {
        // 웨이브 시간 끝 — 다음 웨이브는 StartTime 기준으로 대기
        return;
    }

    // SpawnInterval 기준으로 주기 스폰
    // NextSpawnTime은 절대 시간(게임 전체 경과 기준)
    const float Now = GameMode->GetElapsedTime();
    if (Now >= NextSpawnTime)
    {
        bool bSpawned = TrySpawnMonster(*CurRow);
        if (bSpawned)
        {
            NextSpawnTime = Now + FMath::Max(0.01f, CurRow->SpawnInterval);
        }
        else
        {
            // 스폰 실패(풀 exhausted / spawnlimit)면 약간 지연해서 재시도
            NextSpawnTime = Now + 0.2f;
        }
    }
}

void ADEMonsterSpawnManager::StartWave(int32 WaveIndex)
{


    //
    WaveElapsedTime = 0.0f;

    if (WaveIndex < 0 || WaveIndex >= StageRowNames.Num())
        return;

    FName RowName = StageRowNames[WaveIndex];
    const FDEStageWaveData* WaveData = StageWaveTable->FindRow<FDEStageWaveData>(RowName, TEXT(""));
    if (!WaveData) return;

    UE_LOG(LogTemp, Warning, TEXT("StartWave %d (StartTime %.2f)"), WaveIndex, WaveData->StartTime);

    // 보스 즉시 스폰 (보스가 지정되어 있으면 한 번만)
    if (WaveData->BossClass)
    {
        SpawnBoss(*WaveData);
    }

    // MinimumCount 즉시 보충 (SpawnLimit 고려)
    int32 CurrentActive = ActiveMonsters.Num();
    int32 SpawnLimit = GameMode ? GameMode->GetSpawnLimit() : INT32_MAX;
    int32 Need = FMath::Max(0, WaveData->MinimumCount - CurrentActive);
    int32 CanSpawn = FMath::Max(0, SpawnLimit - CurrentActive);
    int32 ToSpawn = FMath::Min(Need, CanSpawn);

    for (int32 i = 0; i < ToSpawn; ++i)
    {
        if (WaveData->MonsterClasses.Num() == 0) break;
        int32 RandIdx = FMath::RandRange(0, WaveData->MonsterClasses.Num() - 1);
        TSubclassOf<ADEMonsterBase> SelectedClass = WaveData->MonsterClasses[RandIdx];
        FVector SpawnLocation = GetRandomSpawnLocation();
        SpawnFromPool(SelectedClass, SpawnLocation);
    }

    // 다음 스폰 시간 초기화(즉시 스폰 시작 가능)
    NextSpawnTime = GameMode ? GameMode->GetElapsedTime() : 0.0f;
    
}

bool ADEMonsterSpawnManager::TrySpawnMonster(const FDEStageWaveData& WaveData)
{
    const int32 CurrentActiveCount = ActiveMonsters.Num();
    const int32 SpawnLimit = GameMode ? GameMode->GetSpawnLimit() : INT32_MAX;

    // 만약 현재가 SpawnLimit 이상이면 스폰 중지
    if (CurrentActiveCount >= SpawnLimit)
        return false;

    // 주기 스폰은 1마리
    if (WaveData.MonsterClasses.Num() == 0)
        return false;

    int32 RandIdx = FMath::RandRange(0, WaveData.MonsterClasses.Num() - 1);
    TSubclassOf<ADEMonsterBase> SelectedClass = WaveData.MonsterClasses[RandIdx];

    FVector SpawnLocation = GetRandomSpawnLocation();

    SpawnFromPool(SelectedClass, SpawnLocation);
    return true;
}

bool ADEMonsterSpawnManager::SpawnBoss(const FDEStageWaveData& WaveData)
{
    if (!WaveData.BossClass)
        return false;

    FVector SpawnLocation =GetRandomSpawnLocation();
    SpawnFromPool(WaveData.BossClass, SpawnLocation);
    UE_LOG(LogTemp, Warning, TEXT("BOSS : %s SPAWNED"), *WaveData.BossClass->GetName());
    return true;
}

ADEMonsterBase* ADEMonsterSpawnManager::SpawnFromPool(TSubclassOf<ADEMonsterBase> MonsterClass, const FVector& Location)
{
    // Try reuse from pool first (matching subclass is optional: pool contains base class instances)
    for (ADEMonsterBase* Monster : MonsterPool)
    {
        if (!Monster) continue;

        // 기존 ResetForPool() / InitializePool()가 풀에 넣을 때 Hide 시킨다고 가정
        if (Monster->IsHidden())
        {
            // Optionally, you can cast to check if this Monster can be used as MonsterClass,
            // or you can change the actor's properties to become the desired 'type'.
            Monster->SetActorLocation(Location);
            Monster->ResetMonster();    // Show, enable, reset stats
            ActiveMonsters.Add(Monster);

            UE_LOG(LogTemp, Verbose, TEXT("Reused monster from pool: %s"), *Monster->GetName());
            return Monster;
        }
    }

    // Spawn new one
    UWorld* World = GetWorld();
    if (!World) return nullptr;

    ADEMonsterBase* NewMonster = World->SpawnActor<ADEMonsterBase>(MonsterClass, Location, FRotator::ZeroRotator);
    if (!NewMonster) return nullptr;

    // 초기화 및 바인딩
    MonsterPool.Add(NewMonster);
    ActiveMonsters.Add(NewMonster);
    NewMonster->OnMonsterDeath.AddUObject(this, &ADEMonsterSpawnManager::OnMonsterDied);

    UE_LOG(LogTemp, Warning, TEXT("Spawned new monster: %s"), *NewMonster->GetName());
    return NewMonster;

}

void ADEMonsterSpawnManager::ReturnMonsterToPool(ADEMonsterBase* Monster)
{
    if (!Monster) return;

    Monster->ResetForPool();

    Monster->SetActorLocation(FVector::ZeroVector);
    UE_LOG(LogTemp, Warning, TEXT("%s Returned to Pool"),*Monster->GetName());
}



//**************************************** OLD FUNC
//ADEMonsterBase* ADEMonsterSpawnManager::SpawnFromPool(const FVector& SpawnLocation, const FRotator& SpawnRotation)
//{
//    for (ADEMonsterBase* Monster : MonsterPool)
//    {
//        if (!Monster) continue;
//
//        // Find Monsters inactive
//        if (Monster->IsHidden())
//        {
//            FVector MobLocation;
//            MobLocation = SpawnLocation;
//            MobLocation.Z = Monster->GetCapsuleHalfHeight()+0.5f;
//            Monster->SetActorLocation(MobLocation);
//            Monster->SetActorRotation(SpawnRotation);
//
//            Monster->ResetMonster();
//
//            ActiveMonsters.Add(Monster);
//
//            return Monster;
//        }
//    }
//
//    // If no Monster in Pool
//    UE_LOG(LogTemp, Warning, TEXT("Monster Pool exhausted!"));
//    return nullptr;
//}
//
//void ADEMonsterSpawnManager::ReturnMonsterToPool(ADEMonsterBase* Monster)
//{
//    if (!Monster) return;
//
//    Monster->ResetForPool();
//
//    Monster->SetActorLocation(FVector::ZeroVector);
//    UE_LOG(LogTemp, Warning, TEXT("%s Returned to Pool"),*Monster->GetName());
//}
//
//
//void ADEMonsterSpawnManager::InitializePool()
//{
//    if (MonsterClasses.Num() == 0) return;
//
//    UWorld* World = GetWorld();
//    if (!World) return;
//
//    for (int32 i = 0; i < InitialPoolSize; i++)
//    {
//        // Choose Random Monster
//        int32 RandIndex = FMath::RandRange(0, MonsterClasses.Num() - 1);
//        TSubclassOf<ADEMonsterBase> MonsterClass = MonsterClasses[RandIndex];
//
//        // Spawn in Pool
//        FActorSpawnParameters Params;
//        Params.Owner = this;
//        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
//        ADEMonsterBase* Monster = World->SpawnActor<ADEMonsterBase>(
//            MonsterClass,
//            FVector::ZeroVector,      // Temp Location
//            FRotator::ZeroRotator,
//            Params
//        );
//
//        if (Monster)
//        {
//            Monster->SetActorHiddenInGame(true);
//            Monster->SetActorEnableCollision(false);
//            Monster->SetActorTickEnabled(false);
//            Monster->OnMonsterDeath.AddUObject(this, &ADEMonsterSpawnManager::OnMonsterDied);
//            MonsterPool.Add(Monster);
//        }
//    }
//
//    UE_LOG(LogTemp, Warning, TEXT("Monster Pool Initialized: %d"), MonsterPool.Num());
//}

FVector ADEMonsterSpawnManager::GetRandomSpawnLocation()
{
    FVector PlayerLocation = Player->GetActorLocation();
    float SpawnX = PlayerLocation.X + FMath::RandRange(-1600.f, 1600.f);
    float SpawnY = PlayerLocation.Y + FMath::RandRange(-1600.f, 1600.f);

    // 화면 안쪽 위치를 제외하고 밖에서만 나오게
    if (FMath::Abs(SpawnX - PlayerLocation.X) < 800.f)
        SpawnX += (SpawnX > PlayerLocation.X ? 800.f : -800.f);
    if (FMath::Abs(SpawnY - PlayerLocation.Y) < 800.f)
        SpawnY += (SpawnY > PlayerLocation.Y ? 800.f : -800.f);

    float SpawnZ = 0.f;
    return FVector(SpawnX, SpawnY, SpawnZ);
}

//void ADEMonsterSpawnManager::StartNextWave()
//{
//    CurrentWave++;
//    UE_LOG(LogTemp, Warning, TEXT("Wave : %d Start"), CurrentWave);
//    if (CurrentWave > TotalWaves)
//    {
//        return; // 모든 웨이브 종료
//    }
//
//    SpawnedThisWave = 0;
//    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ADEMonsterSpawnManager::SpawnWaveMonster, SpawnInterval, true);
//}

//void ADEMonsterSpawnManager::SpawnWaveMonster()
//{
//    if (SpawnedThisWave >= MonstersPerWave)
//    {
//        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
//        GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ADEMonsterSpawnManager::StartNextWave, WaveInterval, false);
//        return;
//    }
//
//    FVector SpawnLocation = GetRandomSpawnLocation();
//    FRotator SpawnRotation = FRotator::ZeroRotator;
//    ADEMonsterBase* Monster = SpawnFromPool(SpawnLocation, SpawnRotation);
//    if (Monster)
//    {
//        SpawnedThisWave++;
//    }
//}

void ADEMonsterSpawnManager::OnMonsterDied(ADEMonsterBase* Monster)
{
    if (Monster)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s : Died"), *Monster->GetName());

    }

    ActiveMonsters.Remove(Monster);
    UE_LOG(LogTemp, Warning, TEXT("Monster Died, Instantly Removed"));
    ReturnMonsterToPool(Monster);
}

void ADEMonsterSpawnManager::ResolveMonsterOverlap(ADEMonsterBase* A, ADEMonsterBase* B)
{
    if (!A || !B) return;

    FVector PosA = A->GetActorLocation();
    FVector PosB = B->GetActorLocation();

    // 평면(2D)으로 계산
    FVector FlatA = FVector(PosA.X, PosA.Y, 0.f);
    FVector FlatB = FVector(PosB.X, PosB.Y, 0.f);

    FVector Delta = FlatB - FlatA;
    float Dist = Delta.Size();

    float radiusA = A->GetCollisionRadius();
    float radiusB = B->GetCollisionRadius();
    float MinDist = radiusA + radiusB + 2.0f;

    if (Dist < KINDA_SMALL_NUMBER)
    {
        // 겹쳐서 동일 위치이면 랜덤 노즈
        FVector Nudge = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 0.f).GetSafeNormal() * 1.0f;
        A->AddActorWorldOffset(-Nudge, false);
        B->AddActorWorldOffset(Nudge, false);
        return;
    }

    if (Dist < MinDist)
    {
        float Penetration = MinDist - Dist;
        FVector PushDir = Delta / Dist; // A -> B

        FVector PushA = -PushDir * (Penetration * 0.5f);
        FVector PushB = PushDir * (Penetration * 0.5f);

        A->AddActorWorldOffset(PushA, false);
        B->AddActorWorldOffset(PushB, false);

        // 체인 넉백 전파
        float AKnockMag = A->KnockbackVelocity.Size();
        if (AKnockMag > 1.f)
        {
            float Transfer = AKnockMag * ChainKnockbackTransfer;
            B->ApplyKnockback(PushDir, Transfer);
            A->KnockbackVelocity *= 0.5f;
        }

        float BKnockMag = B->KnockbackVelocity.Size();
        if (BKnockMag > 1.f)
        {
            float Transfer = BKnockMag * ChainKnockbackTransfer;
            A->ApplyKnockback(-PushDir, Transfer);
            B->KnockbackVelocity *= 0.5f;
        }
    }
}