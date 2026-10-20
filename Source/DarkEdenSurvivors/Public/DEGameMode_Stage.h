// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DEGameModeBase.h"
#include "DEGameMode_Stage.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEGameMode_Stage : public ADEGameModeBase
{
	GENERATED_BODY()

public:
    ADEGameMode_Stage();
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // 캐릭터 선택: 접속 옵션 "Character=<id>"를 PlayerController에 저장하고, 그 ID의 DT 행 CharacterClass로 스폰한다.
    // 옵션이 없으면 (스테이지 맵을 PIE로 바로 켰을 때 등) DefaultPawnClass
    virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
    virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

    // 스폰된 플레이어 폰의 사망을 게임오버에 연결한다
    virtual void SetPlayerDefaults(APawn* PlayerPawn) override;

public:
    float GetElapsedTime() const { return ElapsedTime; }
    void RegisterMonsterSpawnManager(class ADEMonsterSpawnManager* InManager);
    class ADEMonsterSpawnManager* GetMonsterSpawnManager() { return MonsterSpawnManager; }
    const TArray<class ADEMonsterBase*>& GetActiveMonsters() const;


    int32 GetSpawnLimit() const { return SpawnLimit; }

protected:
    UPROPERTY(VisibleInstanceOnly, Category = "GameInfo")
    float ElapsedTime;
    UPROPERTY(VisibleInstanceOnly, Category = "GameInfo")
    int32 SpawnLimit;
    UPROPERTY(VisibleInstanceOnly, Category = "GameInfo")
    class ADEMonsterSpawnManager* MonsterSpawnManager = nullptr;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
    int32 EarnedGold = 0;

    // 이 시간(초)만큼 버티면 클리어. 0 이하면 클리어 없음 (테스트 맵용)
    UPROPERTY(EditDefaultsOnly, Category = "Stage", meta = (Units = "s"))
    float StageDuration = 1800.0f;

    // 클리어나 게임오버로 스테이지가 끝났다. 정산이 두 번 일어나지 않게 막는다
    UPROPERTY(VisibleInstanceOnly, Category = "GameInfo")
    bool bStageEnded = false;

    

    // 결과창 위젯 클래스 (에디터에서 할당)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> ResultWidgetClass;


public:
    UFUNCTION()
    void GameOver();

    // ElapsedTime이 StageDuration에 도달하면 Tick에서 부른다
    void StageClear();

    bool IsStageEnded() const { return bStageEnded; }

private:
    // 클리어와 게임오버 공통: 정지, 골드 정산과 저장, 결과창
    void EndStage(bool bCleared);

public:
    // ★ 몬스터가 죽거나 상자를 먹었을 때 호출할 '입금' 함수
    UFUNCTION(BlueprintCallable, Category = "Reward")
    void AddGold(int32 Amount);
    // ... 나머지 매니저 포인터도 선언
};
