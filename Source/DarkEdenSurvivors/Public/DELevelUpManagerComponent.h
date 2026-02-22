// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DELevelUpManagerComponent.generated.h"

class UDELevelUpChoiceBase;
class UDEAccessoryData;
class UDataTable;

struct FAccessoryPoolItem
{
    class UDEAccessoryData* Data;
    float Weight;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKEDENSURVIVORS_API UDELevelUpManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDELevelUpManagerComponent();

    TArray<UDELevelUpChoiceBase*> GenerateChoices(int32 Count);
    void ApplyChoice(UDELevelUpChoiceBase* Choice);

protected:
    virtual void BeginPlay() override;

private:
    // ===== 설정값 =====

    // 스킬 확률 (0~1)
    UPROPERTY(EditAnywhere, Category = "LevelUp")
    float SkillWeight = 0.7f;

    UPROPERTY(EditAnywhere, Category = "LevelUp")
    float AccessoryWeight = 0.3f;

    // [변경] 데이터 테이블 자체를 C++에서 보관
    UPROPERTY()
    UDataTable* AccessoryDataTable;

    // [추가] 시작 시 데이터 테이블을 싹 다 긁어와서 캐싱해둘 마스터 풀
    TArray<FAccessoryPoolItem> MasterAccessoryPool;

private:
    UDELevelUpChoiceBase* CreateRandomSkillChoice();
    UDELevelUpChoiceBase* CreateRandomAccessoryChoice();


};
