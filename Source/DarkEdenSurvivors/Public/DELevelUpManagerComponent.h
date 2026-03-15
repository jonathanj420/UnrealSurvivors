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


public:
    // 상자를 열었을 때 호출될 핵심 함수! (최종적으로 1, 3, 5개의 보상을 배열로 반환)
    UFUNCTION(BlueprintCallable, Category = "Chest")
    TArray<UDELevelUpChoiceBase*> GenerateChestRewards(float PlayerLuck);

private:
    // 1단계: 상자 등급(개수) 굴리기 (1개, 3개, 5개)
    int32 CalculateChestJackpot(float Luck);

    // 2단계: 현재 가진 스킬 중 진화 가능한 녀석이 있는지 탐색
    UDELevelUpChoiceBase* TryGetEvolutionChoice();

    // 3단계: 현재 가진 스킬/악세 중 '만렙이 아닌 녀석' 하나를 랜덤으로 뽑기
    UDELevelUpChoiceBase* GetRandomUpgradableChoice();

    // 4단계: 다 만렙일 때 줄 꽝 보상 (돈 주머니, 체력 회복 등)
    UDELevelUpChoiceBase* GetFallbackChoice();


};
