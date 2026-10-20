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

    

    // 결과창 위젯 클래스 (에디터에서 할당)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> ResultWidgetClass;


public:
    UFUNCTION()
    void GameOver();
public:
    // ★ 몬스터가 죽거나 상자를 먹었을 때 호출할 '입금' 함수
    UFUNCTION(BlueprintCallable, Category = "Reward")
    void AddGold(int32 Amount);
    // ... 나머지 매니저 포인터도 선언
};
