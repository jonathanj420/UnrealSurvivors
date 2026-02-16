// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "Engine/GameInstance.h"
#include "Data/DEMonsterData.h"
#include "DESaveGame.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "DEGameInstance.generated.h"

/**
 * 
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

	// === [골드 관리 API] ===
	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetTotalGold() const;

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddGold(int32 Amount); // 골드 획득

	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool TryConsumeGold(int32 Amount); // 골드 사용 (성공 시 true)

	// === [업그레이드 관리 API] ===
	// 특정 스탯의 현재 레벨 가져오기
	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetUpgradeLevel(FName StatName) const;

	// 특정 스탯 레벨업 시키기 (골드 차감 로직은 UI에서 TryConsumeGold 후 호출 권장)
	UFUNCTION(BlueprintCallable, Category = "Progression")
	void LevelUpStat(FName StatName);

public:
	// 사람이 읽을 수 있는 JSON 저장
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGameToJSON();

	// JSON 파일 불러오기
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGameFromJSON();

protected:
	UPROPERTY()
	UDESaveGame* CurrentSaveData; // 메모리에 로드된 세이브 데이터 캐싱

	const FString SAVE_SLOT_NAME = TEXT("DESaveSlot01");




public:
    // 외부에서 몬스터 ID만 주면 데이터를 뱉어주는 함수
    const FDEMonsterData* GetMonsterData(FName MonsterID);

protected:
    // 에디터에서 할당할 원본 데이터 테이블
    UPROPERTY(EditDefaultsOnly, Category = "Data")
    TObjectPtr<UDataTable> SourceMonsterTable;

private:
    // 게임 실행 시 파싱해서 들고 있을 캐시 메모리 (Map 구조)
    TMap<FName, FDEMonsterData> MonsterDataCache;
};
