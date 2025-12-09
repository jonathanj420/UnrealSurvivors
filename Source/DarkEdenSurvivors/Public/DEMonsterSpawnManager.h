// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DEMonsterSpawnManager.generated.h"


USTRUCT()
struct FMonsterWaveEntry
{
    GENERATED_BODY()

    TSubclassOf<ADEMonsterBase> MonsterClass;
    int32 Count;
};

USTRUCT()
struct FMonsterWave
{
    GENERATED_BODY()

    TArray<FMonsterWaveEntry> Entries;
};

UCLASS()
class DARKEDENSURVIVORS_API ADEMonsterSpawnManager : public AActor
{
    GENERATED_BODY()

public:
    ADEMonsterSpawnManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;



    //************* Monster Pool**************
    //Monster classes to spawn
    UPROPERTY(EditAnywhere, Category = "Pooling")
    TArray<TSubclassOf<class ADEMonsterBase>> MonsterClasses;
    //Pooled Monster Instance
    UPROPERTY(EditAnywhere, Category = "Pooling")
    TArray<class ADEMonsterBase*> MonsterPool;

    UFUNCTION()
    ADEMonsterBase* SpawnFromPool(const FVector& SpawnLocation, const FRotator& SpawnRotation = FRotator::ZeroRotator);
    UFUNCTION()
    void ReturnMonsterToPool(class ADEMonsterBase* Monster);

    UPROPERTY(EditAnywhere, Category = "Pooling")
    int32 InitialPoolSize = 50;

    
    void InitializePool();


    //************* Monster Wave***********
    // Spawn Rate(Interval)
    UPROPERTY(EditAnywhere, Category = "Wave")
    TArray<class ADEMonsterBase*> ActiveMonsters;
    UPROPERTY(EditAnywhere, Category = "Wave")
    float SpawnInterval = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Wave")
    int32 MonstersPerWave = 5;

    UPROPERTY(EditAnywhere, Category = "Wave")
    float WaveInterval = 5.0f; // Wave Interval

    UPROPERTY(EditAnywhere, Category = "Wave")
    int32 TotalWaves = 3;

    int32 CurrentWave = 0;
    int32 SpawnedThisWave = 0;
    FTimerHandle SpawnTimerHandle;
    FTimerHandle WaveTimerHandle;

    FVector GetRandomSpawnLocation();

    void StartNextWave();
    void SpawnWaveMonster();
    //**** MONSTER COLLISION****
    UPROPERTY(EditAnywhere, Category = "Collision")
    float MinSeparationDistance = 80.f; // 몬스터간 최소 거리 (반지름*2 정도)

    UPROPERTY(EditAnywhere, Category = "Collision")
    float ChainKnockbackTransfer = 0.5f; // 앞->뒤로 전달 비율

    void ResolveMonsterOverlap(ADEMonsterBase* A, ADEMonsterBase* B);


public:
    void OnMonsterDied(class ADEMonsterBase* Monster);


    // 실제 스폰하는 함수
    //void SpawnMonster();

    // 플레이어 참조
    class ADEFemaleVampire* Player;

private:


};
