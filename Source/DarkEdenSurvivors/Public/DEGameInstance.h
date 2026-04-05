// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "Engine/GameInstance.h"
#include "Data/DEMonsterData.h"
#include "DEStatTypes.h"
#include "DESaveGame.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "DEGameInstance.generated.h"

/**
 * 게임의 전역 상태 및 메타 프로그레션(영구 강화) 정보를 관리하는 GameInstance 클래스입니다.
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
    UDEGameInstance();
    virtual void Init() override;

	// === [저장/로드 시스템] ===
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGame();

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGame();

	// === [재화 관련 API] ===
	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetTotalGold() const;

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddGold(int32 Amount); // 골드 획득

	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool TryConsumeGold(int32 Amount); // 골드 소비 (성공 시 true)

	// === [업그레이드 관련 API] ===
	// 특정 스태트의 현재 강화 레벨 가져오기
	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetUpgradeLevel(EDEStatType StatType) const;

	// 특정 스태트 강화 시 필요한 비용 가져오기
	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetUpgradeCost(EDEStatType StatType) const;

	// 특정 스태트 레벨업 시도 (골드 소모 포함)
	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool TryLevelUpStat(EDEStatType StatType);

	// 게임 내에서 적용할 실제 보너스 수치 가져오기
	UFUNCTION(BlueprintPure, Category = "Progression")
	float GetStatUpgradeBonus(EDEStatType StatType) const;

public:
	// 디버그용 JSON 저장/로드
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGameToJSON();

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGameFromJSON();

protected:
	UPROPERTY()
	UDESaveGame* CurrentSaveData; // 메모리에 로드된 세이브 데이터

	const FString SAVE_SLOT_NAME = TEXT("DESaveSlot01");

public:
    // 몬스터 데이터를 가져오는 함수
    const FDEMonsterData* GetMonsterData(FName MonsterID);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Data")
    TObjectPtr<UDataTable> SourceMonsterTable;

private:
    TMap<FName, FDEMonsterData> MonsterDataCache;
};
