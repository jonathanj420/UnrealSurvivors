// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEMonsterUpdateComponent.generated.h"

class ADEMonsterBase;
class ADECharacterBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEMonsterUpdateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDEMonsterUpdateComponent();

    // SpawnManager의 Tick에서 호출
    void UpdateMonsters(float DeltaTime, TArray<ADEMonsterBase*>& ActiveMonsters);

    // 플레이어 참조 (SpawnManager에서 주입)
    void SetPlayer(ADECharacterBase* InPlayer) { Player = InPlayer; }

    // ===== 충돌 설정 =====
    UPROPERTY(EditAnywhere, Category = "Collision")
    float ChainKnockbackTransfer = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Collision")
    float SoftPushRangeMultiplier = 1.3f;

    UPROPERTY(EditAnywhere, Category = "Collision")
    float SoftPushStrength = 1.0f;

private:
    void UpdateMovementAndAttack(float DeltaTime, TArray<ADEMonsterBase*>& ActiveMonsters);
    void ResolveOverlaps(TArray<ADEMonsterBase*>& ActiveMonsters);

    void ResolveMonsterOverlap(ADEMonsterBase* A, ADEMonsterBase* B,
        const FVector& PosA, const FVector& PosB);
    void ResolvePlayerPush(ADEMonsterBase* Mob, const FVector& PlayerLocation, const FVector& MobLocation, float PlayerRadius);

    UPROPERTY()
    ADECharacterBase* Player = nullptr;

//Grid Spatial Partitioning
private:
    // 1. 셀 사이즈는 기획자가 에디터에서 튜닝할 수 있게 UPROPERTY로 빼주는 게 국룰!
    UPROPERTY(EditAnywhere, Category = "Optimization")
    float CellSize = 200.0f;

    // 2. 매 프레임 재활용할 공간 분할 바구니 (절대 Tick 안에서 생성하지 않음!)
    TMap<FIntPoint, TArray<ADEMonsterBase*>> SpatialGrid;

    TSet<FIntPoint> LastFrameTouchedCells;

    // [최적화 #1] 프레임당 GetActorLocation() 중복 호출 제거용 캐시
    // SetNumUninitialized()로 크기만 맞추고 재사용 → 매 프레임 힙 할당 없음
    TArray<FVector> CachedPositions;

		
};
