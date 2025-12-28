// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameInstance.h"
#include "UObject/ConstructorHelpers.h" // 필수 헤더

UDEGameInstance::UDEGameInstance()
{
    // [C++ 하드코딩 로드 방식]
    // TEXT("...") 안에 아까 복사한 경로를 붙여넣으세요.
    // 주의: 경로 앞의 불필요한 타입명(/Script/Engine.DataTable')은 지우고 
    //       /Game/... 부분만 남기는 게 깔끔합니다.

    static ConstructorHelpers::FObjectFinder<UDataTable> DT_MonsterAsset(
        TEXT("/Game/DarkEden/Data/Monster/DT_MonsterData.DT_MonsterData")
    );

    if (DT_MonsterAsset.Succeeded())
    {
        SourceMonsterTable = DT_MonsterAsset.Object;
        UE_LOG(LogTemp, Warning, TEXT("GameInstance: DataTable Loaded via C++ Code."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance: Failed to Load DataTable! Check Path."));
    }
}

void UDEGameInstance::Init()
{
    Super::Init();

    if (SourceMonsterTable)
    {
        // 1. 테이블의 모든 행(Row)을 가져옴
        TArray<FName> RowNames = SourceMonsterTable->GetRowNames();

        for (const FName& RowName : RowNames)
        {
            // 2. 데이터 추출
            FDEMonsterData* Data = SourceMonsterTable->FindRow<FDEMonsterData>(RowName, TEXT("GameInstance Init"));

            if (Data)
            {
                // 3. 내 캐시(Map)에 저장
                MonsterDataCache.Add(RowName, *Data);
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("GameInstance: Cached %d Monsters."), MonsterDataCache.Num());
    }
}

const FDEMonsterData* UDEGameInstance::GetMonsterData(FName MonsterID)
{
    // 이제 테이블 뒤질 필요 없이 Map에서 즉시 리턴 (가장 빠름)
    if (MonsterDataCache.Contains(MonsterID))
    {
        return &MonsterDataCache[MonsterID];
    }

    return nullptr;
}