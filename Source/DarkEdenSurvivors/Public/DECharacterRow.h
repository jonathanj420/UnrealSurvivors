// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DECharacterRow.generated.h"

class UTexture2D;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDECharacterRow : public FTableRowBase
{
    GENERATED_BODY()

    // 1. 캐릭터 식별용 고유 ID
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    int32 CharacterID = 0;

    // 2. 캐릭터 이름 (예: "안토니오", "다크에덴 뱀파이어")
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    FText CharacterName;

    // 3. 고유 패시브/특성 설명 (예: "10레벨마다 데미지가 10% 증가합니다.")
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Info")
    FText Description;

    // 4. 캐릭터 초상화 (로비 선택창 및 인게임 UI에 쓰일 이미지)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    UTexture2D* Portrait = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class")
    TSubclassOf<class ADECharacterBase> CharacterClass;
};