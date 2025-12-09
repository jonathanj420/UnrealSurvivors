// Fill out your copyright notice in the Description page of Project Settings.


#include "DEMonsterSpawnManager.h"
#include "DEMonsterBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DEFemaleVampire.h"

ADEMonsterSpawnManager::ADEMonsterSpawnManager()
{
    PrimaryActorTick.bCanEverTick = true;
    //MonsterClass = ADEMonsterBase::StaticClass();
    MonsterClasses.Add(ADEMonsterBase::StaticClass());
    //MonsterClasses.Add(ADEMonsterBase::StaticClass());
    //MonsterClasses.Add(ADEMonsterBase::StaticClass()); . . . . . 

    USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootSceneComponent;
}

void ADEMonsterSpawnManager::BeginPlay()
{
    Super::BeginPlay();
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PlayerController)
    {
        APawn* PlayerPawn = PlayerController->GetPawn();

        Player = Cast<ADEFemaleVampire>(PlayerPawn);

    }

    InitializePool();

    StartNextWave();
}

void ADEMonsterSpawnManager::Tick(float DeltaTime)
{
    //control all monsters' logic here
    Super::Tick(DeltaTime);

    if (!Player) return;

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

ADEMonsterBase* ADEMonsterSpawnManager::SpawnFromPool(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
    for (ADEMonsterBase* Monster : MonsterPool)
    {
        if (!Monster) continue;

        // Find Monsters inactive
        if (Monster->IsHidden())
        {
            FVector MobLocation;
            MobLocation = SpawnLocation;
            MobLocation.Z = Monster->GetCapsuleHalfHeight()+0.5f;
            Monster->SetActorLocation(MobLocation);
            Monster->SetActorRotation(SpawnRotation);

            Monster->ResetMonster();

            ActiveMonsters.Add(Monster);

            return Monster;
        }
    }

    // If no Monster in Pool
    UE_LOG(LogTemp, Warning, TEXT("Monster Pool exhausted!"));
    return nullptr;
}

void ADEMonsterSpawnManager::ReturnMonsterToPool(ADEMonsterBase* Monster)
{
    if (!Monster) return;

    Monster->ResetForPool();

    Monster->SetActorLocation(FVector::ZeroVector);
    UE_LOG(LogTemp, Warning, TEXT("%s Returned to Pool"),*Monster->GetName());
}


void ADEMonsterSpawnManager::InitializePool()
{
    if (MonsterClasses.Num() == 0) return;

    UWorld* World = GetWorld();
    if (!World) return;

    for (int32 i = 0; i < InitialPoolSize; i++)
    {
        // Choose Random Monster
        int32 RandIndex = FMath::RandRange(0, MonsterClasses.Num() - 1);
        TSubclassOf<ADEMonsterBase> MonsterClass = MonsterClasses[RandIndex];

        // Spawn in Pool
        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        ADEMonsterBase* Monster = World->SpawnActor<ADEMonsterBase>(
            MonsterClass,
            FVector::ZeroVector,      // Temp Location
            FRotator::ZeroRotator,
            Params
        );

        if (Monster)
        {
            Monster->SetActorHiddenInGame(true);
            Monster->SetActorEnableCollision(false);
            Monster->SetActorTickEnabled(false);
            Monster->OnMonsterDeath.AddUObject(this, &ADEMonsterSpawnManager::OnMonsterDied);
            MonsterPool.Add(Monster);
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("Monster Pool Initialized: %d"), MonsterPool.Num());
}

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

void ADEMonsterSpawnManager::StartNextWave()
{
    CurrentWave++;
    UE_LOG(LogTemp, Warning, TEXT("Wave : %d Start"), CurrentWave);
    if (CurrentWave > TotalWaves)
    {
        return; // 모든 웨이브 종료
    }

    SpawnedThisWave = 0;
    GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ADEMonsterSpawnManager::SpawnWaveMonster, SpawnInterval, true);
}

void ADEMonsterSpawnManager::SpawnWaveMonster()
{
    if (SpawnedThisWave >= MonstersPerWave)
    {
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ADEMonsterSpawnManager::StartNextWave, WaveInterval, false);
        return;
    }

    FVector SpawnLocation = GetRandomSpawnLocation();
    FRotator SpawnRotation = FRotator::ZeroRotator;
    ADEMonsterBase* Monster = SpawnFromPool(SpawnLocation, SpawnRotation);
    if (Monster)
    {
        SpawnedThisWave++;
    }
}

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