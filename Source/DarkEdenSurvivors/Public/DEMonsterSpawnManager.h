// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Data/DEStageWaveData.h"    
#include "Data/DEMonsterData.h"
#include "Engine/StreamableManager.h" // 상단 인클루드 추가
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
    UPROPERTY()
    TMap<FName, TSubclassOf<ADEMonsterBase>> CachedMonsterClasses;

    // 비동기 로딩을 도와줄 엔진 매니저
    FStreamableManager StreamableManager;

    // 로딩된 에셋들이 가비지 컬렉터(GC)에 날아가지 않게 꽉 잡고 있을 손잡이
    TSharedPtr<FStreamableHandle> PreloadHandle;

    // 프리로드 함수 선언
    void PreloadMonsterResources();

    // 활성화되어 필드에 있는 몬스터 목록
    UPROPERTY(VisibleInstanceOnly, Category = "Pooling")
    TArray<ADEMonsterBase*> ActiveMonsters;


    // 죽어서 대기 중인(꺼져있는) 몬스터 목록
    UPROPERTY(VisibleInstanceOnly, Category = "Pooling")
    TArray<ADEMonsterBase*> InactiveMonsters;

    TArray<ADEMonsterBase*> PendingRemoveMonsters;
    TArray<ADEMonsterBase*> MonstersToProcess;

    ADEMonsterBase* SpawnFromPool(FVector& Location, const struct FDEMonsterData* DataToApply, FName MonsterID);
    UFUNCTION()
    void ReturnMonsterToPool(class ADEMonsterBase* Monster);

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
    // 현재 웨이브 몬스터가 MinimumCount보다 적으면 그 수까지 즉시 스폰
    void ReplenishToMinimum(const FDEStageWaveData& WaveData);
    bool SpawnBoss(const FDEStageWaveData& WaveData);

    // 현재 웨이브가 스폰한 생존 몬스터 (MinimumCount 판정용). 웨이브 시작 시 초기화
    TSet<ADEMonsterBase*> CurrentWaveMonsters;

    // 생존 중인 보스. ActiveMonsters에는 있지만 SpawnLimit 계산에서는 제외
    TSet<ADEMonsterBase*> ActiveBosses;

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
