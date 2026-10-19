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
DECLARE_MULTICAST_DELEGATE(FOnLevelUpQueueFinished);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKEDENSURVIVORS_API UDELevelUpManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDELevelUpManagerComponent();

    // 레벨이 하나 오를 때마다 부른다. 여러 번 쌓여도 선택은 하나씩 진행되고,
    // 선택지는 직전 선택이 적용된 뒤의 상태로 만든다
    void EnqueueLevelUp();

    // 현재 선택지에 없는 Choice(이전 선택지, 중복 클릭)는 무시하고 false
    bool ApplyChoice(UDELevelUpChoiceBase* Choice);

    bool IsSelectingLevelUp() const { return !CurrentChoices.IsEmpty(); }

    FOnLevelUpChoicesReady OnLevelUpChoicesReady;
    // 쌓인 레벨업 선택이 모두 끝났을 때. 선택지를 한 번도 띄우지 않았으면 오지 않는다
    FOnLevelUpQueueFinished OnLevelUpQueueFinished;

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
    UDELevelUpChoiceBase* GetRandomUpgradableChoice(const TArray<int32>& ExcludedSkillIDs);

    // 4단계: 다 만렙일 때 줄 꽝 보상 (돈 주머니, 체력 회복 등)
    UDELevelUpChoiceBase* GetFallbackChoice();


};
