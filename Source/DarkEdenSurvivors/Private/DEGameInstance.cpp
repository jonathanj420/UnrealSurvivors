// Fill out your copyright notice in the Description page of Project Settings.

#include "DEGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Kismet/GameplayStatics.h"

UDEGameInstance::UDEGameInstance()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> DT_MonsterAsset(
        TEXT("/Game/DarkEden/Data/Monster/DT_MonsterData.DT_MonsterData")
    );

    if (DT_MonsterAsset.Succeeded())
    {
        SourceMonsterTable = DT_MonsterAsset.Object;
    }
}

void UDEGameInstance::Init()
{
    Super::Init();
    LoadGame();
    
    if (SourceMonsterTable)
    {
        TArray<FName> RowNames = SourceMonsterTable->GetRowNames();
        for (const FName& RowName : RowNames)
        {
            FDEMonsterData* Data = SourceMonsterTable->FindRow<FDEMonsterData>(RowName, TEXT("GameInstance Init"));
            if (Data)
            {
                MonsterDataCache.Add(RowName, *Data);
            }
        }
    }
}

void UDEGameInstance::SaveGame()
{
	if (!CurrentSaveData) return;
	UGameplayStatics::AsyncSaveGameToSlot(CurrentSaveData, SAVE_SLOT_NAME, 0);
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] Game Saved. Gold: %d"), CurrentSaveData->TotalGold);
}

void UDEGameInstance::LoadGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SAVE_SLOT_NAME, 0))
	{
		CurrentSaveData = Cast<UDESaveGame>(UGameplayStatics::LoadGameFromSlot(SAVE_SLOT_NAME, 0));
		UE_LOG(LogTemp, Log, TEXT("[GameInstance] Save Loaded. Gold: %d"), CurrentSaveData->TotalGold);
	}
	else
	{
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
}

bool UDEGameInstance::TryConsumeGold(int32 Amount)
{
	if (!CurrentSaveData) return false;

	if (CurrentSaveData->TotalGold >= Amount)
	{
		CurrentSaveData->TotalGold -= Amount;
		SaveGame();
		return true;
	}
	return false;
}

int32 UDEGameInstance::GetUpgradeLevel(EDEStatType StatType) const
{
	if (!CurrentSaveData) return 0;

	switch (StatType)
	{
	case EDEStatType::Damage: return CurrentSaveData->UpgradeStatus.DamageMultiplierLevel;
	case EDEStatType::MaxHP: return CurrentSaveData->UpgradeStatus.HealthMultiplierLevel;
	case EDEStatType::MoveSpeed: return CurrentSaveData->UpgradeStatus.MoveSpeedMultiplierLevel;
	case EDEStatType::Amount: return CurrentSaveData->UpgradeStatus.BonusAmountLevel;
	case EDEStatType::Greed: return CurrentSaveData->UpgradeStatus.GreedLevel;
	default: return 0;
	}
}

int32 UDEGameInstance::GetUpgradeCost(EDEStatType StatType) const
{
    int32 Level = GetUpgradeLevel(StatType);
    // 기본 비용 500골드, 레벨당 500골드씩 증가하는 간단한 공식
    return (Level + 1) * 500;
}

bool UDEGameInstance::TryLevelUpStat(EDEStatType StatType)
{
    if (!CurrentSaveData) return false;

    int32 Cost = GetUpgradeCost(StatType);
    if (TryConsumeGold(Cost))
    {
        switch (StatType)
        {
        case EDEStatType::Damage: CurrentSaveData->UpgradeStatus.DamageMultiplierLevel++; break;
        case EDEStatType::MaxHP: CurrentSaveData->UpgradeStatus.HealthMultiplierLevel++; break;
        case EDEStatType::MoveSpeed: CurrentSaveData->UpgradeStatus.MoveSpeedMultiplierLevel++; break;
        case EDEStatType::Amount: CurrentSaveData->UpgradeStatus.BonusAmountLevel++; break;
        case EDEStatType::Greed: CurrentSaveData->UpgradeStatus.GreedLevel++; break;
        default: return false;
        }
        SaveGame();
        return true;
    }

    return false;
}

float UDEGameInstance::GetStatUpgradeBonus(EDEStatType StatType) const
{
    int32 Level = GetUpgradeLevel(StatType);
    if (Level <= 0) return 0.0f;

    switch (StatType)
    {
    case EDEStatType::Damage:
    case EDEStatType::MaxHP:
    case EDEStatType::MoveSpeed:
    case EDEStatType::Greed:
        // 퍼센트 수치: 레벨당 +5% (0.05)
        return Level * 0.05f;

    case EDEStatType::Amount:
        // 가산 수치: 레벨당 +1개
        return (float)Level;

    default:
        return 0.0f;
    }
}

void UDEGameInstance::SaveGameToJSON()
{
    if (!CurrentSaveData) return;
    TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    FJsonObjectConverter::UStructToJsonObject(CurrentSaveData->GetClass(), CurrentSaveData, JsonObject);

    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject, Writer);

    FString SavePath = FPaths::ProjectSavedDir() + TEXT("SaveGames/SaveData.json");
    if (FFileHelper::SaveStringToFile(JsonString, *SavePath))
    {
        UE_LOG(LogTemp, Warning, TEXT("JSON Saved to: %s"), *SavePath);
    }
}

void UDEGameInstance::LoadGameFromJSON()
{
    FString SavePath = FPaths::ProjectSavedDir() + TEXT("SaveGames/SaveData.json");
    if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*SavePath)) return;

    FString JsonString;
    if (!FFileHelper::LoadFileToString(JsonString, *SavePath)) return;

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
    {
        if (!CurrentSaveData)
        {
            CurrentSaveData = Cast<UDESaveGame>(UGameplayStatics::CreateSaveGameObject(UDESaveGame::StaticClass()));
        }
        FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), CurrentSaveData->GetClass(), CurrentSaveData, 0, 0);
        UE_LOG(LogTemp, Warning, TEXT("JSON Loaded! Gold: %d"), CurrentSaveData->TotalGold);
    }
}

const FDEMonsterData* UDEGameInstance::GetMonsterData(FName MonsterID)
{
    if (MonsterDataCache.Contains(MonsterID))
    {
        return &MonsterDataCache[MonsterID];
    }
    return nullptr;
}
