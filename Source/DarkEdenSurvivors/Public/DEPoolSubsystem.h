// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DEPoolSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEPoolSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** 풀에서 액터를 꺼내거나 필요 시 생성 */
    AActor* GetPooledActor(
        TSubclassOf<AActor> ActorClass,
        const FVector& Location,
        const FRotator& Rotation,
        bool bAutoActivate = true
    );

    /** 액터를 풀로 돌려보내는 함수 */
    void ReturnActorToPool(AActor* Actor);
    /** 미리 액터를 풀에 채우는 기능 */
    //void PreloadActors(TSubclassOf<AActor> ActorClass, int32 Count);
private:

    /** 클래스별 풀 저장소 */
    TMap<TSubclassOf<AActor>, TArray<AActor*>> PoolMap;

    /** 클래스 역조회용: Actor → Class */
    TMap<AActor*, TSubclassOf<AActor>> ActorClassMap;

    /** 풀 Shrink 타이머 핸들 */
    FTimerHandle ShrinkTimerHandle;

    /** ----- 내부 유틸 함수 ----- */

    /** 풀에 새 액터 생성 */
    AActor* CreateNewPooledActor(TSubclassOf<AActor> ActorClass, UWorld* World);

    /** Actor가 "비활성 상태(재사용 가능)"인지 판단 */
    bool IsActorInactive(AActor* Actor) const;

    /** Actor 활성화 */
    void ActivateActor(
        AActor* Actor,
        const FVector& Location,
        const FRotator& Rotation,
        bool bAutoActivate
    );

    /** Actor 비활성화 */
    void DeactivateActor(AActor* Actor);

    /** 자동 축소 실행 */
    void ShrinkPools();

    /** 월드 가져오기 */
    UWorld* GetWorldSafe() const;

    /** 클래스별 최대 풀 크기 (기본 50) */
    UPROPERTY(EditAnywhere, Category = "Pooling")
    int32 MaxPoolSize = 50;

    /** 자동 축소 기능 켜기 */
    UPROPERTY(EditAnywhere, Category = "Pooling")
    bool bEnableAutoShrink = true;

    /** 몇 초마다 Shrink 체크할지 */
    UPROPERTY(EditAnywhere, Category = "Pooling")
    float ShrinkCheckInterval = 60.0f;

    /** Shrink 시 풀 크기를 이 수까지 줄임 */
    UPROPERTY(EditAnywhere, Category = "Pooling")
    int32 MinPoolSize = 100;

	
};
