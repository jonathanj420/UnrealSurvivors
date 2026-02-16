// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DEMonsterRepositionManager.generated.h"


class ADEMonsterSpawnManager;
class ADEMonsterBase;
class ACharacter;


UCLASS()
class DARKEDENSURVIVORS_API ADEMonsterRepositionManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADEMonsterRepositionManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
    /* ===== Reference ===== */

    UPROPERTY()
    class ADEGameMode_Stage* GameMode;

    UPROPERTY()
    ADEMonsterSpawnManager* MonsterSpawnManager;

    UPROPERTY()
    ACharacter* Player;

    /* ===== Config ===== */

    // 이 거리보다 멀어지면 재배치
    UPROPERTY(EditAnywhere, Category = "Reposition")
    float MaxDistanceFromPlayer = 2000.0f;

    // 재배치 시 플레이어 기준 최소 거리
    UPROPERTY(EditAnywhere, Category = "Reposition")
    float RespawnMinRadius = 1000.0f;

    // 재배치 시 플레이어 기준 최대 거리
    UPROPERTY(EditAnywhere, Category = "Reposition")
    float RespawnMaxRadius = 1100.0f;

private:
    void RepositionMonster(ADEMonsterBase* Monster);
    FVector GetRandomPositionAroundPlayer() const;

};
