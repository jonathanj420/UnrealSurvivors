// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DELevelUpManagerComponent.generated.h"

class UDELevelUpChoiceBase;
class UDELevelUpChoice_Skill;
class UDEAccessoryData;
class UDataTable;

struct FAccessoryPoolItem
{
    class UDEAccessoryData* Data;
    float Weight;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUpChoicesReady, const TArray<UDELevelUpChoiceBase*>& /*Choices*/);
DECLARE_MULTICAST_DELEGATE(FOnLevelUpClosed);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnChestRewardsReady, const TArray<UDELevelUpChoiceBase*>& /*Rewards*/);
DECLARE_MULTICAST_DELEGATE(FOnChestClosed);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKEDENSURVIVORS_API UDELevelUpManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDELevelUpManagerComponent();

    // 레벨업과 상자는 한 번에 하나만 띄운다. 같은 프레임에 겹치면 레벨업을 먼저 처리하고,
    // 다음 선택지나 보상은 직전 것이 적용된 뒤의 상태로 만든다

    // 레벨이 하나 오를 때마다 부른다
    void EnqueueLevelUp();

    // 현재 선택지에 없는 Choice(이전 선택지, 중복 클릭)는 무시하고 false
    bool ApplyChoice(UDELevelUpChoiceBase* Choice);

    bool IsSelectingLevelUp() const { return !CurrentChoices.IsEmpty(); }

    FOnLevelUpChoicesReady OnLevelUpChoicesReady;
    // 레벨업 창을 닫을 때 (쌓인 레벨업을 다 골랐을 때). 선택지를 한 번도 띄우지 않았으면 오지 않는다
    FOnLevelUpClosed OnLevelUpClosed;

    // 상자를 주웠을 때 부른다
    void EnqueueChest(float PlayerLuck);

    // 열려 있는 상자의 보상을 모두 적용한다. 열린 상자가 없으면 false
    bool ClaimChestRewards();

    bool IsChestOpen() const { return bChestOpen; }

    FOnChestRewardsReady OnChestRewardsReady;
    // 상자 창을 닫을 때 (다음 상자가 없거나, 기다리던 레벨업을 먼저 띄울 때)
    FOnChestClosed OnChestClosed;

protected:
    virtual void BeginPlay() override;

private:
    TArray<UDELevelUpChoiceBase*> GenerateChoices(int32 Count);

    // 선택지를 띄웠으면 true. 올릴 게 없는 레벨업은 건너뛴다
    bool PresentNextLevelUp();

    UPROPERTY(EditAnywhere, Category = "LevelUp")
    int32 ChoiceCount = 3;

    int32 PendingLevelUpCount = 0;

    // 선택지 객체는 여기서만 강하게 참조된다
    UPROPERTY()
    TArray<UDELevelUpChoiceBase*> CurrentChoices;

    // 상자를 열었으면 true. 보상 생성은 이전 상자 보상이 적용된 뒤의 상태로 한다
    bool OpenNextChest();

    TArray<float> QueuedChestLuck;

    // 보상이 0개인 상자도 열린 상태가 있으므로 배열과 따로 둔다
    bool bChestOpen = false;

    UPROPERTY()
    TArray<UDELevelUpChoiceBase*> CurrentChestRewards;

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
    UDELevelUpChoice_Skill* TryGetEvolutionChoice();

    // 3단계: 현재 가진 스킬/악세 중 '만렙이 아닌 녀석' 하나를 랜덤으로 뽑기
    // 상자 보상은 한꺼번에 적용되므로, 같은 상자에서 앞서 뽑힌 레벨업(Planned*)을 현재 레벨에 더해서 본다. 뽑은 것은 Planned*에 더한다
    UDELevelUpChoiceBase* GetRandomUpgradableChoice(const TArray<int32>& ExcludedSkillIDs,
        TMap<int32, int32>& PlannedSkillLevels, TMap<const UDEAccessoryData*, int32>& PlannedAccessoryLevels);

    // 4단계: 다 만렙일 때 줄 꽝 보상 (돈 주머니, 체력 회복 등)
    UDELevelUpChoiceBase* GetFallbackChoice();


};
