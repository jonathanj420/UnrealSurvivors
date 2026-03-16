// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Data/DEStageWaveData.h"    
#include "Data/DEMonsterData.h"
#include "DEMonsterSpawnManager.generated.h"


class ADEGameMode_Stage;
class UDEMonsterUpdateComponent;
class ADEMonsterBase;
class UDEGameInstance;

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

    // 활성화되어 필드에 있는 몬스터 목록
    UPROPERTY(VisibleInstanceOnly, Category = "Pooling")
    TArray<ADEMonsterBase*> ActiveMonsters;


    // 죽어서 대기 중인(꺼져있는) 몬스터 목록 (기존 MonsterPool 대체)
    UPROPERTY(VisibleInstanceOnly, Category = "Pooling")
    TArray<ADEMonsterBase*> InactiveMonsters;

    bool bIsResolvingOverlaps = false;

    TArray<ADEMonsterBase*> PendingRemoveMonsters;


    ADEMonsterBase* SpawnFromPool(FVector& Location, const struct FDEMonsterData* DataToApply);
    UFUNCTION()
    void ReturnMonsterToPool(class ADEMonsterBase* Monster);

    UPROPERTY(EditAnywhere, Category = "Pooling")
    int32 InitialPoolSize = 50;

  

    //**************** New Wave System**********
    UPROPERTY(EditAnywhere, Category = "Wave")
    UDataTable* StageWaveTable;

    TArray<FName> StageRowNames;

    UPROPERTY(VisibleAnywhere, Category = "Wave")
    int32 CurrentWaveIndex;

    UPROPERTY(VisibleAnywhere, Category = "Wave")
    float WaveElapsedTime;

    UPROPERTY(VisibleAnywhere, Category = "Wave")
    float NextSpawnTime;

    UPROPERTY()
    ADEGameMode_Stage* GameMode;

    void ProcessWave(float DeltaTime);
    void StartWave(int32 WaveIndex);
    bool TrySpawnMonster(const FDEStageWaveData& WaveData);
    bool SpawnBoss(const FDEStageWaveData& WaveData);
    int KillCount = 0;

    //************* Monster Wave***********
    // Spawn Rate(Interval)
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


public:
    void OnMonsterDied(class ADEMonsterBase* Monster);
    const TArray<ADEMonsterBase*>& GetActiveMonsters() const;


    // 플레이어 참조
    class ADECharacterBase* Player;
    // 몬스터 참조
    TSubclassOf<ADEMonsterBase> MonsterBase;
private:
    UPROPERTY()
    TObjectPtr<UDEGameInstance> GameInstanceCache;

    // UPROPERTY 추가
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UDEMonsterUpdateComponent* MonsterUpdateComponent;

};
