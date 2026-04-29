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

    // =========================================================
    // ★ [핵심] 이미 뽑힌 녀석들을 기록해둘 '블랙리스트 장부'
    // =========================================================
    TArray<int32> PickedSkillIDs;
    TArray<const UDEAccessoryData*> PickedAccs;

    int32 MaxAttempts = Count * 10; // (안전장치) 최대 시도 횟수
    int32 Attempts = 0;

    // 장바구니가 꽉 차거나, 최대 시도 횟수를 넘길 때까지 무한 반복!
    while (Result.Num() < Count && Attempts < MaxAttempts)
    {
        Attempts++;
        float Roll = FMath::FRand();

        if (Roll < SkillWeight)
        {
            if (UDELevelUpChoiceBase* RawChoice = CreateRandomSkillChoice())
            {
                UDELevelUpChoice_Skill* SkillChoice = Cast<UDELevelUpChoice_Skill>(RawChoice);

                // 장부에 없는(처음 뽑힌) 스킬이라면?
                if (SkillChoice && !PickedSkillIDs.Contains(SkillChoice->GetSkillID()))
                {
                    PickedSkillIDs.Add(SkillChoice->GetSkillID()); // 장부에 기록
                    Result.Add(SkillChoice);                  // 결과 바구니에 담기
                    UE_LOG(LogTemp, Warning, TEXT("Random Skill Selected: %s"), *SkillChoice->GetDisplayName().ToString());
                }
            }
        }
        else
        {
            if (UDELevelUpChoiceBase* RawChoice = CreateRandomAccessoryChoice())
            {
                UDELevelUpChoice_Accessory* AccChoice = Cast<UDELevelUpChoice_Accessory>(RawChoice);

                // 장부에 없는(처음 뽑힌) 장신구라면?
                if (AccChoice && !PickedAccs.Contains(AccChoice->GetAccessoryData()))
                {
                    PickedAccs.Add(AccChoice->GetAccessoryData()); // 장부에 기록
                    Result.Add(AccChoice);                    // 결과 바구니에 담기
                    UE_LOG(LogTemp, Warning, TEXT("Random Accessory Selected: %s"), *AccChoice->GetDisplayName().ToString());
                }
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
    UDESkillManagerComponent* SkillMgr = GetOwner()->FindComponentByClass<UDESkillManagerComponent>();
    if (!SkillMgr) return nullptr;

    // 1. 랜덤 스킬의 기본 정보(Row)를 가져옵니다 (아이콘, 이름 등을 위해 필수!)
    const FDESkillRow* Row = SkillMgr->GetRandomSkillRow();
    if (!Row) return nullptr;

    // 2. 플레이어의 현재 스킬 레벨을 확인합니다. (SkillMgr에 이런 함수가 있다고 가정)
    int32 CurrentLevel = SkillMgr->GetSkillLevel(Row->SkillID);
    int32 TargetLevel = CurrentLevel + 1;

    FText DisplayDescription; // UI에 최종적으로 띄워줄 텍스트

    // 3. 레벨에 따라 텍스트를 다르게 가져옵니다.
    if (CurrentLevel == 0)
    {
        // 처음 먹는 스킬일 때
        DisplayDescription = Row->BaseDescription;
    }
    else
    {
        // 이미 있는 스킬 레벨업일 때 (FDESkillData 테이블 조회)
        // (가정: SkillID와 TargetLevel을 주면 해당 행을 뱉어내는 함수)
        const FDESkillData* DataRow = SkillMgr->GetSkillDataRow(Row->SkillID, TargetLevel);

        if (DataRow)
        {
            DisplayDescription = DataRow->UpgradeDescription;
        }
        else
        {
            // 혹시라도 데이터를 못 찾거나 만렙인 경우의 안전장치
            DisplayDescription = FText::FromString(TEXT("최대 레벨 도달!"));
        }
    }

    // 4. 초이스 객체 생성
    UDELevelUpChoice_Skill* Choice = NewObject<UDELevelUpChoice_Skill>(this);

    // ★ Choice->Init 함수를 수정해서 설명과 레벨도 같이 넘겨줍니다!
    Choice->Init(
        Row->SkillID,
        Row->SkillName,
        Row->SkillIcon,
        DisplayDescription, // 위에서 결정한 텍스트
        TargetLevel         // UI에서 "Lv.2" 같이 띄워주기 위해 넘김
    );

    return Choice;

    /*UDESkillManagerComponent* SkillMgr =
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

    return Choice;*/
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

TArray<UDELevelUpChoiceBase*> UDELevelUpManagerComponent::GenerateChestRewards(float PlayerLuck)
{
    TArray<UDELevelUpChoiceBase*> ChestRewards;

    // 1. 몇 개 뽑을지 잭팟 룰렛 돌리기 (1, 3, 5 반환)
    int32 RewardCount = CalculateChestJackpot(PlayerLuck);
    UE_LOG(LogTemp, Warning, TEXT("[Chest] Chest Reward Count: %d"), RewardCount);

    // 2. [최우선 순위] 진화 가능한 스킬이 있는지 확인!
    if (UDELevelUpChoiceBase* EvolutionChoice = TryGetEvolutionChoice())
    {
        ChestRewards.Add(EvolutionChoice);
        RewardCount--; // 진화 스킬이 한 자리를 차지함!
        UE_LOG(LogTemp, Warning, TEXT("[Chest] Evolution complete"));
    }

    // 3. 남은 자리 채우기 (기존에 가진 스킬/악세 중에서만 레벨업)
    for (int32 i = 0; i < RewardCount; ++i)
    {
        if (UDELevelUpChoiceBase* UpgradeChoice = GetRandomUpgradableChoice())
        {
            ChestRewards.Add(UpgradeChoice);
        }
        else
        {
            // 4. 더 이상 올릴 스킬/악세가 없다면 꽝(돈/힐) 보상으로 대체!
            ChestRewards.Add(GetFallbackChoice());
        }
    }

    // 완성된 보상 목록을 반환! (이걸 위젯에 넘겨서 1, 3, 5개 연출을 띄우면 됨)
    return ChestRewards;
}

int32 UDELevelUpManagerComponent::CalculateChestJackpot(float Luck)
{
    // 뱀서식 간단 룰렛 예시
    float Roll = FMath::FRandRange(0.0f, 100.0f);

    // Luck 스탯에 따라 확률이 유동적으로 변함!
    if (Roll < (1.0f + Luck * 0.1f)) return 5; // 금상자! (기본 1% 확률)
    if (Roll < (10.0f + Luck * 0.5f)) return 3; // 은상자! (기본 10% 확률)

    return 1; // 기본 나무상자
}


UDELevelUpChoiceBase* UDELevelUpManagerComponent::TryGetEvolutionChoice()
{
    UE_LOG(LogTemp, Warning, TEXT("[Chest] Try Get Evolution/Synthesis Choice"));
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return nullptr;

    UDESkillManagerComponent* SkillMgr = OwnerPawn->FindComponentByClass<UDESkillManagerComponent>();
    if (!SkillMgr) return nullptr;

    // ★ 단일 ID 대신 배열을 준비합니다.
    TArray<int32> ConsumedSkillIDs;
    int32 ResultSkillID = -1;

    // 1. 네가 짠 그 완벽한 진화/합성 판독기 작동!
    if (SkillMgr->CheckEvolution(ConsumedSkillIDs, ResultSkillID))
    {
        // 2. 판독 통과! 완성될 결과 스킬(ResultSkillID)의 데이터 로드
        const FDESkillRow* EvolvedRow = SkillMgr->GetSkillRow(ResultSkillID);
        if (EvolvedRow)
        {
            // 3. Choice 객체 포장해서 반환
            UDELevelUpChoice_Skill* EvolutionChoice = NewObject<UDELevelUpChoice_Skill>(this);

            EvolutionChoice->Init(
                EvolvedRow->SkillID,
                EvolvedRow->SkillName,
                EvolvedRow->SkillIcon,
                EvolvedRow->BaseDescription
            );

            // =========================================================
            // ★ 중요: 이제 이 카드는 자기가 어떤 재료들을 갈아넣어야 하는지 배열로 기억합니다!
            // =========================================================
            EvolutionChoice->bIsEvolution = true;
            EvolutionChoice->ConsumedSkillIDs = ConsumedSkillIDs;

            UE_LOG(LogTemp, Warning, TEXT("[Chest] Synthesis Ready! %d skills will be consumed to make -> %d"),
                ConsumedSkillIDs.Num(), ResultSkillID);

            return EvolutionChoice;
        }
    }

    return nullptr; // 진화/합성할 거 없음

    //UE_LOG(LogTemp, Warning, TEXT("[Chest] Try Get Evolution Choice"));
    //APawn* OwnerPawn = Cast<APawn>(GetOwner());
    //if (!OwnerPawn) return nullptr;

    //UDESkillManagerComponent* SkillMgr = OwnerPawn->FindComponentByClass<UDESkillManagerComponent>();
    //if (!SkillMgr) return nullptr;


    //UE_LOG(LogTemp, Warning, TEXT("[Chest] Evolution Criteria Passed"));

    //int32 BaseSkillID = -1;
    //int32 ResultSkillID = -1;

    //// 1. 네가 짠 그 완벽한 진화 판독기 작동!
    //if (SkillMgr->CheckEvolution(BaseSkillID, ResultSkillID))
    //{
    //    // 2. 판독 통과! 진화될 결과 스킬(ResultSkillID)의 데이터 로드
    //    UE_LOG(LogTemp, Warning, TEXT("[Chest] Passed CheckEvolution"));
    //    const FDESkillRow* EvolvedRow = SkillMgr->GetSkillRow(ResultSkillID);
    //    if (EvolvedRow)
    //    {
    //        // 3. Choice 객체 포장해서 반환
    //        UDELevelUpChoice_Skill* EvolutionChoice = NewObject<UDELevelUpChoice_Skill>(this);

    //        EvolutionChoice->Init(
    //            EvolvedRow->SkillID,
    //            EvolvedRow->SkillName,
    //            EvolvedRow->SkillIcon,
    //            EvolvedRow->BaseDescription

    //        );

    //        // =========================================================
    //        // ★ 중요 팁: 이 Choice가 '진화용'이라는 걸 나중에 Apply할 때 알 수 있어야 해!
    //        // =========================================================
    //         EvolutionChoice->bIsEvolution = true;
    //         EvolutionChoice->TargetBaseSkillID = BaseSkillID; 

    //        UE_LOG(LogTemp, Warning, TEXT("[Chest] Evolved %d -> %d"), BaseSkillID, ResultSkillID);

    //        return EvolutionChoice;
    //    }
    //}

    //return nullptr; // 진화할 거 없음
}

UDELevelUpChoiceBase* UDELevelUpManagerComponent::GetRandomUpgradableChoice()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return nullptr;

    UDESkillManagerComponent* SkillMgr = OwnerPawn->FindComponentByClass<UDESkillManagerComponent>();
    UDEAccessoryComponent* AccComp = OwnerPawn->FindComponentByClass<UDEAccessoryComponent>();

    if (!SkillMgr || !AccComp) return nullptr;

    // 1. 만렙이 아닌 후보들을 담을 빈 바구니 준비
    TArray<int32> UpgradableSkillIDs;
    TArray<const UDEAccessoryData*> UpgradableAccessories;

    // =========================================================
    // 2. 무기 바구니 채우기 (가진 것 중 만렙 아닌 것만)
    // =========================================================
    // (GetActiveSkills()가 현재 장착 중인 스킬 배열을 반환한다고 가정)
    for (const auto& Pair : SkillMgr->GetActiveSkills())
    {
        // Pair.Key는 SkillID, Pair.Value는 FActiveSkill 구조체라고 가정
        int32 SkillID = Pair.Key;
        int32 CurrentLevel = SkillMgr->GetSkillLevels().Contains(SkillID) ? SkillMgr->GetSkillLevels()[SkillID] : 1;

        // 데이터 테이블에서 MaxLevel 가져오기
        const FDESkillRow* Row = SkillMgr->GetSkillRow(SkillID);
        if (Row && CurrentLevel < Row->MaxLevel)
        {
            UpgradableSkillIDs.Add(SkillID);
        }
    }

    // =========================================================
    // 3. 장신구 바구니 채우기 (가진 것 중 만렙 아닌 것만)
    // =========================================================
    const TMap<const UDEAccessoryData*, int32>& EquippedAcc = AccComp->GetEquippedAccessories();
    for (const auto& Pair : EquippedAcc)
    {
        const UDEAccessoryData* AccData = Pair.Key;
        int32 CurrentLevel = Pair.Value;

        if (AccData && CurrentLevel < AccData->MaxLevel)
        {
            UpgradableAccessories.Add(AccData);
        }
    }

    // =========================================================
    // 4. 총 후보 개수 파악 & 랜덤 뽑기!
    // =========================================================
    int32 TotalCandidates = UpgradableSkillIDs.Num() + UpgradableAccessories.Num();

    // 올릴 게 하나도 없다! (전부 만렙 ㄷㄷ) -> nullptr 반환해서 꽝(Fallback) 처리로 넘김
    if (TotalCandidates == 0) return nullptr;

    // 0부터 (후보 개수 - 1) 사이에서 랜덤으로 핑퐁!
    int32 RandomIndex = FMath::RandRange(0, TotalCandidates - 1);

    // =========================================================
    // 5. 당첨된 녀석을 Choice 캡슐로 예쁘게 포장해서 반환
    // =========================================================
    if (RandomIndex < UpgradableSkillIDs.Num())
    {
        // [무기 당첨!] 랜덤 인덱스가 무기 배열 크기 안쪽이면 무기임
        int32 PickedSkillID = UpgradableSkillIDs[RandomIndex];
        const FDESkillRow* Row = SkillMgr->GetSkillRow(PickedSkillID);

        // ★ [추가] 현재 레벨 파악 & 다음 레벨 데이터 가져오기
        int32 CurrentLevel = SkillMgr->GetSkillLevel(PickedSkillID);
        int32 TargetLevel = CurrentLevel + 1;

        FText DisplayDescription;
        const FDESkillData* DataRow = SkillMgr->GetSkillDataRow(PickedSkillID, TargetLevel);
        if (DataRow)
        {
            DisplayDescription = DataRow->UpgradeDescription;
        }
        else
        {
            DisplayDescription = FText::FromString(TEXT("None"));
        }

        // ★ [수정] 새로 바뀐 Init 함수 규격에 맞춰서 포장!
        UDELevelUpChoice_Skill* Choice = NewObject<UDELevelUpChoice_Skill>(this);
        Choice->Init(
            Row->SkillID,
            Row->SkillName,
            Row->SkillIcon,
            DisplayDescription, // 텍스트 추가
            TargetLevel         // 다음 레벨 추가
        );
        return Choice;
    }
    else
    {
        // [장신구 당첨!] 무기 개수를 뺀 나머지 인덱스면 장신구임
        int32 AccIndex = RandomIndex - UpgradableSkillIDs.Num();
        const UDEAccessoryData* PickedAcc = UpgradableAccessories[AccIndex];

        UDELevelUpChoice_Accessory* Choice = NewObject<UDELevelUpChoice_Accessory>(this);
        Choice->Init(PickedAcc);
        return Choice;
    }
}

UDELevelUpChoiceBase* UDELevelUpManagerComponent::GetFallbackChoice()
{
    return nullptr;
}
