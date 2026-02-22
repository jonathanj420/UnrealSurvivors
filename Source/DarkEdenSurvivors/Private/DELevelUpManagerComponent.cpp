// Fill out your copyright notice in the Description page of Project Settings.


#include "DELevelUpManagerComponent.h"
#include "DELevelUpChoice_Skill.h"
#include "DELevelUpChoice_Accessory.h"
#include "DESkillManagerComponent.h"
#include "DEAccessoryComponent.h"
#include "DEAccessoryData.h"
#include "Kismet/KismetMathLibrary.h"

UDELevelUpManagerComponent::UDELevelUpManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    static ConstructorHelpers::FObjectFinder<UDataTable> DT_Acc(TEXT("/Game/DarkEden/Data/Accessories/DEAccessoryRowTable.DEAccessoryRowTable"));
    if (DT_Acc.Succeeded())
    {
        AccessoryDataTable = DT_Acc.Object;
    }

}

void UDELevelUpManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    // 2. 게임 시작 시 한 번만! 테이블 전체(GetAllRows)를 읽어서 마스터 배열에 캐싱해 둡니다.
    if (AccessoryDataTable)
    {
        TArray<FDEAccessoryRow*> AllRows;
        AccessoryDataTable->GetAllRows<FDEAccessoryRow>(TEXT("AccessoryLoadContext"), AllRows);

        for (FDEAccessoryRow* Row : AllRows)
        {
            if (Row && Row->AccessoryData)
            {
                // 데이터 에셋과 가중치를 묶어서 추가
                MasterAccessoryPool.Add({ Row->AccessoryData, Row->DropWeight });
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("[LevelUpManager] Loaded %d Accessories from DataTable."), MasterAccessoryPool.Num());
    }
}

TArray<UDELevelUpChoiceBase*> UDELevelUpManagerComponent::GenerateChoices(int32 Count)
{
    TArray<UDELevelUpChoiceBase*> Result;

    for (int32 i = 0; i < Count; ++i)
    {
        float Roll = FMath::FRand();

        if (Roll < SkillWeight)
        {
            if (UDELevelUpChoiceBase* Choice = CreateRandomSkillChoice())
            {
                Result.Add(Choice);
                UE_LOG(LogTemp, Warning, TEXT("Random Choice : %s"), *Choice->GetName());
            }
        }
        else
        {
            if (UDELevelUpChoiceBase* Choice = CreateRandomAccessoryChoice())
            {
                Result.Add(Choice);
                UE_LOG(LogTemp, Warning, TEXT("Random Choice : %s"), *Choice->GetName());
            }
        }
    }

    return Result;
}

void UDELevelUpManagerComponent::ApplyChoice(UDELevelUpChoiceBase* Choice)
{
    if (!Choice)
        return;

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
        return;

    // ★ 핵심: 실제 적용은 Choice가 한다
    Choice->Apply(OwnerActor);
}

UDELevelUpChoiceBase* UDELevelUpManagerComponent::CreateRandomSkillChoice()
{
    UDESkillManagerComponent* SkillMgr =
        GetOwner()->FindComponentByClass<UDESkillManagerComponent>();

    if (!SkillMgr)
        return nullptr;

    const FDESkillRow* Row = SkillMgr->GetRandomSkillRow();
    if (!Row)
        return nullptr;

    UDELevelUpChoice_Skill* Choice =
        NewObject<UDELevelUpChoice_Skill>(this);

    Choice->Init(
        Row->SkillID,
        Row->SkillName,
        Row->SkillIcon
    );

    return Choice;
}

UDELevelUpChoiceBase* UDELevelUpManagerComponent::CreateRandomAccessoryChoice()
{
    // 데이터가 없으면 진행 불가
    if (MasterAccessoryPool.IsEmpty()) return nullptr;

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return nullptr;

    UDEAccessoryComponent* AccComp = OwnerPawn->FindComponentByClass<UDEAccessoryComponent>();
    if (!AccComp) return nullptr;

    // 1. 플레이어 상태 파악
    const TMap<const UDEAccessoryData*, int32>& EquippedMap = AccComp->GetEquippedAccessories();
    int32 MaxSlots = 6; // 악세서리 최대 6칸
    bool bIsSlotFull = (EquippedMap.Num() >= MaxSlots);

    // 2. 등장 가능한 유효 후보군(ValidCandidates) 추리기 및 전체 가중치 합산
    TArray<FAccessoryPoolItem> ValidCandidates;
    float TotalWeight = 0.0f;

    for (const FAccessoryPoolItem& Item : MasterAccessoryPool)
    {
        const int32* CurrentLevelPtr = EquippedMap.Find(Item.Data);

        // [조건 A] 이미 가진 템인데 만렙이 아니다 -> 등장 가능!
        if (CurrentLevelPtr != nullptr && *CurrentLevelPtr < Item.Data->MaxLevel)
        {
            ValidCandidates.Add(Item);
            TotalWeight += Item.Weight;
        }
        // [조건 B] 아직 없는 템인데 슬롯에 빈자리가 있다 -> 등장 가능!
        else if (CurrentLevelPtr == nullptr && !bIsSlotFull)
        {
            ValidCandidates.Add(Item);
            TotalWeight += Item.Weight;
        }
    }

    // 뽑을 게 아무것도 없다면 (전부 만렙이거나 자리 꽉 참)
    if (ValidCandidates.IsEmpty() || TotalWeight <= 0.0f)
    {
        // 나중에는 여기서 '골드 주머니'나 '고기(체력회복)'로 대체 반환하면 됩니다.
        return nullptr;
    }

    // 3. ★ 대망의 가중치 기반 랜덤 룰렛 돌리기 ★
    float RandomRoll = FMath::FRandRange(0.0f, TotalWeight);
    UDEAccessoryData* SelectedData = nullptr;

    for (const FAccessoryPoolItem& Candidate : ValidCandidates)
    {
        RandomRoll -= Candidate.Weight;

        if (RandomRoll <= 0.0f)
        {
            SelectedData = Candidate.Data;
            break;
        }
    }

    // (안전장치) 부동소수점 오차로 안 뽑혔을 경우 마지막 항목 지정
    if (!SelectedData)
    {
        SelectedData = ValidCandidates.Last().Data;
    }

    // 4. 최종적으로 뽑힌 데이터를 Choice 객체에 담아서 반환
    UDELevelUpChoice_Accessory* Choice = NewObject<UDELevelUpChoice_Accessory>(this);
    Choice->Init(SelectedData);

    return Choice;
}