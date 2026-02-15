// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "Subsystems/WorldSubsystem.h"
#include "DEPickupManager.generated.h"

class ADEPickupBase;
class UDEStatComponent;
/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEPickupManager : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UDEPickupManager();

	// [Subsystem Lifecycle]
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// [FTickableGameObject Interface]
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return !IsTemplate(); }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UDEPickupManager, STATGROUP_Tickables); }

	// [Public API]
	// 1. 아이템 생성 요청
	void SpawnPickup(const FVector& Location, int32 ExpValue, float Luck = 0.0f);

	// 2. 전역 자석 발동 (자석 아이템 먹었을 때)
	void TriggerGlobalMagnet(AActor* TargetPlayer);

	// 3. 모든 아이템 청소 (스테이지 종료 시)
	void ClearAllPickups();

protected:
	// 내부 함수: 특정 아이템을 자석 리스트에 등록
	void RegisterMagnetRequest(ADEPickupBase* Pickup, AActor* Target);

protected:
	// [State]
	// 현재 월드에 깔린 모든 아이템 (장부)
	UPROPERTY()
	TArray<TWeakObjectPtr<ADEPickupBase>> ActivePickups;

	// 현재 플레이어에게 빨려오고 있는 아이템 목록 (이동 처리용)
	UPROPERTY()
	TArray<TWeakObjectPtr<ADEPickupBase>> MagnetizingPickups;

	// 플레이어 스탯 컴포넌트 캐싱 (매번 찾기 방지)
	UPROPERTY()
	TWeakObjectPtr<UDEStatComponent> CachedStatComponent;

	// [Settings]
	// 아이템 클래스들 (C++ StaticClass로 로드)
	UPROPERTY()
	TSubclassOf<ADEPickupBase> ExpGemClass;
	UPROPERTY()
	TSubclassOf<ADEPickupBase> PotionClass;
	UPROPERTY()
	TSubclassOf<ADEPickupBase> MagnetItemClass;

	// 최적화용 제한 개수
	const int32 MaxActivePickups = 400;

};
