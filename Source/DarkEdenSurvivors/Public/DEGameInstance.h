// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "Engine/GameInstance.h"
#include "Data/DEMonsterData.h"
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
