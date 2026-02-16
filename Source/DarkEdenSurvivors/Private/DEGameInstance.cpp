// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameInstance.h"
#include "UObject/ConstructorHelpers.h" // 필수 헤더
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

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
    LoadGame();
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

void UDEGameInstance::SaveGame()
{
	if (!CurrentSaveData) return;

	// 비동기 저장 (Async) - 게임 렉 방지 필수
	UGameplayStatics::AsyncSaveGameToSlot(CurrentSaveData, SAVE_SLOT_NAME, 0);

	UE_LOG(LogTemp, Log, TEXT("[GameInstance] Game Saved. Gold: %d"), CurrentSaveData->TotalGold);
}

void UDEGameInstance::LoadGame()
{
	// 슬롯에 파일이 있는지 확인
	if (UGameplayStatics::DoesSaveGameExist(SAVE_SLOT_NAME, 0))
	{
		CurrentSaveData = Cast<UDESaveGame>(UGameplayStatics::LoadGameFromSlot(SAVE_SLOT_NAME, 0));
		UE_LOG(LogTemp, Log, TEXT("[GameInstance] Save Loaded."));
	}
	else
	{
		// 없으면 새로 생성
		CurrentSaveData = Cast<UDESaveGame>(UGameplayStatics::CreateSaveGameObject(UDESaveGame::StaticClass()));
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] New Save Created."));
	}
}

int32 UDEGameInstance::GetTotalGold() const
{
	return CurrentSaveData ? CurrentSaveData->TotalGold : 0;
}

void UDEGameInstance::AddGold(int32 Amount)
{
	if (!CurrentSaveData) return;

	CurrentSaveData->TotalGold += Amount;
	// 중요: 돈 먹을 때마다 저장하면 IO 부하 심함. 게임 오버나 상점 종료 시에만 SaveGame 호출 권장.
}

bool UDEGameInstance::TryConsumeGold(int32 Amount)
{
	if (!CurrentSaveData) return false;

	if (CurrentSaveData->TotalGold >= Amount)
	{
		CurrentSaveData->TotalGold -= Amount;
		SaveGame(); // 돈 썼으면 바로 저장하는 게 안전함
		return true;
	}
	return false;
}

int32 UDEGameInstance::GetUpgradeLevel(FName StatName) const
{
	if (!CurrentSaveData) return 0;

	// 이름으로 분기 (나중에 Enum으로 바꾸면 더 안전함)
	/*if (StatName == FName("Attack")) return CurrentSaveData->UpgradeStatus.DamageMultiplierLevel;
	if (StatName == FName("Health")) return CurrentSaveData->UpgradeStatus.MaxHealthLevel;
	if (StatName == FName("Speed")) return CurrentSaveData->UpgradeStatus.MoveSpeedLevel;
	if (StatName == FName("Greed")) return CurrentSaveData->UpgradeStatus.GreedLevel;*/

	return 0;
}

void UDEGameInstance::LevelUpStat(FName StatName)
{
	if (!CurrentSaveData) return;

	/*if (StatName == FName("Attack")) CurrentSaveData->UpgradeStatus.AttackDamageLevel++;
	else if (StatName == FName("Health")) CurrentSaveData->UpgradeStatus.MaxHealthLevel++;
	else if (StatName == FName("Speed")) CurrentSaveData->UpgradeStatus.MoveSpeedLevel++;
	else if (StatName == FName("Greed")) CurrentSaveData->UpgradeStatus.GreedLevel++;*/

	SaveGame(); // 강화 직후 저장
}

void UDEGameInstance::SaveGameToJSON()
{
    if (!CurrentSaveData) return;

    // [수정 1] 빈 JSON 객체를 먼저 만든다 (MakeShared)
    TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();

    // [수정 2] StaticStruct() 대신 GetClass() 사용
    // 함수 형태: UStructToJsonObject(클래스정보, 인스턴스, 채울_JSON_객체)
    FJsonObjectConverter::UStructToJsonObject(CurrentSaveData->GetClass(), CurrentSaveData, JsonObject);

    // JSON 객체를 문자열(String)로 변환
    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject, Writer);

    // 파일로 저장
    FString SavePath = FPaths::ProjectSavedDir() + TEXT("SaveGames/SaveData.json");

    if (FFileHelper::SaveStringToFile(JsonString, *SavePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("JSON Saved to: %s"), *SavePath);
    }
}

void UDEGameInstance::LoadGameFromJSON()
{
    FString SavePath = FPaths::ProjectSavedDir() + TEXT("SaveGames/SaveData.json");

    // 파일 확인
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*SavePath))
    {
        UE_LOG(LogTemp, Error, TEXT("JSON File not found!"));
        return;
    }

    // 파일 읽기
    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *SavePath))
    {
        return;
    }

    // 문자열 -> JSON 객체 파싱
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        if (!CurrentSaveData)
        {
            CurrentSaveData = Cast<UDESaveGame>(UGameplayStatics::CreateSaveGameObject(UDESaveGame::StaticClass()));
        }

        // [수정 3] StaticStruct() 대신 GetClass() 사용
        // JSON 내용을 내 SaveGame 인스턴스에 덮어씌우기
        FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), CurrentSaveData->GetClass(), CurrentSaveData, 0, 0);

        UE_LOG(LogTemp, Warning, TEXT("JSON Loaded! Gold: %d"), CurrentSaveData->TotalGold);

        // 중요: 불러온 내용을 바이너리 파일(.sav)로도 동기화하고 싶다면 여기서 SaveGame() 호출
        // SaveGame(); 
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