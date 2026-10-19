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

    // 캡슐 반지름 합 + 이 값 이내면 접촉으로 보고 공격한다
    UPROPERTY(EditAnywhere, Category = "Collision")
    float AttackContactTolerance = 10.0f;

private:
    void UpdateMovementAndAttack(float DeltaTime, TArray<ADEMonsterBase*>& ActiveMonsters);
    void ResolveOverlaps(TArray<ADEMonsterBase*>& ActiveMonsters);

    void ResolveMonsterOverlap(ADEMonsterBase* A, ADEMonsterBase* B,
        const FVector& PosA, const FVector& PosB);
    // 플레이어와 겹치면 밀어내고, 접촉 거리 안이면 true
    bool ResolvePlayerPush(ADEMonsterBase* Mob, const FVector& PlayerLocation, const FVector& MobLocation, float PlayerRadius);

    UPROPERTY()
    ADECharacterBase* Player = nullptr;

    // 한 프레임에 적용하는 이동/밀림량 상한. 그리드를 만든 뒤 이만큼 움직이므로 쿼리 패딩에도 쓴다
    static constexpr float MaxPushPerFrame = 20.0f;
};
