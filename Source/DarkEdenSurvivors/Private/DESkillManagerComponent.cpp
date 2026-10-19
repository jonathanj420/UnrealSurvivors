// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillManagerComponent.h"
#include "DEAutoSkillBase.h"
#include "DECharacterBase.h"
#include "DEStatComponent.h"
#include "DEInventoryComponent.h"
#include "Engine/World.h"
#include "DarkEdenSurvivors.h"

// Sets default values for this component's properties
UDESkillManagerComponent::UDESkillManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
    LoadSkillRowTable();
    LoadSkillDataTable();
    LoadEvolutionDataTable();

	// ...
}


// Called when the game starts
void UDESkillManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    InitSkills();

}

void UDESkillManagerComponent::ApplyCharacterDamageMultiplier(float Multiplier)
{
}


// Called every frame
void UDESkillManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 1. 일시정지 체크
    if (bAutoSkillPaused)
        return;

    for (auto& Pair : ActiveSkills)
    {
        FActiveSkill& Active = Pair.Value;
        if (!Active.RowData) continue;

        // =========================================================
        // ★ [핵심 2] 스킬이 아직 공전(유지) 중이라면 쿨타임을 깎지 않고 스킵!
        // =========================================================
        if (Active.SkillObject && Active.SkillObject->IsRunning())
        {
            continue;
        }

        // 아직 쿨타임이 남았다면 깎고 다음 스킬로 넘어감
        if (Active.CurrentCooldown > 0.f)
        {
            Active.CurrentCooldown -= DeltaTime;
            continue;
        }

        // 쿨타임이 다 돌았고, 스킬 데이터가 유효하다면 발사!
        if (Active.SkillObject && Active.RowData)
        {

            // =========================================================
            // ★ [수정 1] 먼저! 다음 발사를 위한 새 쿨타임부터 장전합니다.
            // =========================================================
            float BaseCooldown = Active.RowData->Cooldown;
            float FinalCooldown = FMath::Max(0.1f, BaseCooldown * (1.0f - CachedCDR));
            //UE_LOG(LogTemp, Error, TEXT("Final Cooldown for Skill : %s = %f"),*Active.SkillObject->GetName(),FinalCooldown);

            // (아까 만든 영수증 발급)
           // Active.CalculatedMaxCooldown = FinalCooldown;

            // 초과 시간(Over-tick) 보정
            Active.CurrentCooldown = FinalCooldown + Active.CurrentCooldown;

            // 만약 초과 보정 때문에 쿨이 또 0 이하로 내려가면 안전하게 리셋
            if (Active.CurrentCooldown <= 0.0f)
            {
                Active.CurrentCooldown = FinalCooldown;
            }

            // =========================================================
            // ★ [수정 2] 쿨타임 세팅이 다 끝난 깨끗한 상태에서 발사!
            // =========================================================
            // 이제 이 안에서 OnKill이 터져서 ReduceCooldown이 불려도,
            // 방금 세팅된 '새 쿨타임'에서 정상적으로 깎이게 됩니다!
            Active.SkillObject->SetCurrentFinalCooldown(FinalCooldown);
            Active.SkillObject->Activate();

        }
    }


}

void UDESkillManagerComponent::LoadSkillRowTable()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> DT_SkillRow(
        TEXT("/Game/DarkEden/Data/Skill/DESkillRowTable.DESkillRowTable")
    );

    if (DT_SkillRow.Succeeded())
    {
        SkillRowTable = DT_SkillRow.Object;
        UE_LOG(LogTemp, Error, TEXT("Skill Row Table Loaded"));
    }
    else
    {
        // 실패 시 로그 출력
        UE_LOG(LogTemp, Error, TEXT("Failed to load Skill Row Table"));
    }
    SkillRowMap.Empty();

    if (!SkillRowTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("SkillRowTable not assigned"));
        return;
    }

    TArray<FName> RowNames = SkillRowTable->GetRowNames();

    for (const FName& RowName : RowNames)
    {
        FDESkillRow* Row = SkillRowTable->FindRow<FDESkillRow>(RowName, TEXT("LoadSkillRowTable"));
        if (!Row) continue;

        SkillRowMap.Add(Row->SkillID, *Row);
    }

    UE_LOG(LogTemp, Log, TEXT("SkillRowTable loaded: %d entries"), SkillRowMap.Num());
}

void UDESkillManagerComponent::LoadSkillDataTable()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> DT_SkillData(
        TEXT("/Game/DarkEden/Data/Skill/DESkillDataTable.DESkillDataTable")
    );

    if (DT_SkillData.Succeeded())
    {
        SkillDataTable = DT_SkillData.Object;
        UE_LOG(LogTemp, Error, TEXT("DataTable Loaded"));
    }
    else
    {
        // 실패 시 로그 출력
        UE_LOG(LogTemp, Error, TEXT("Failed to load Skill Data Table"));
    }
}



// 스킬 객체 생성
void UDESkillManagerComponent::InitSkills()
{
    InitializedSkills.Empty();
    if (!SkillDataTable) return;

    static const FString Context(TEXT("SkillLoad"));
    TArray<FName> RowNames = SkillDataTable->GetRowNames();

    for (auto& RowName : RowNames)
    {
        FDESkillData* Row = SkillDataTable->FindRow<FDESkillData>(RowName, Context); \
        if (!Row) continue;
        //UE_LOG(LogTemp, Warning, TEXT("Row %s_%d Found"), *Row->SkillName, Row->Level);


        int32 SkillID = Row->SkillID;   // RowName 사용 안 하는 방식
        int32 Level = Row->Level;

        InitializedSkills.FindOrAdd(SkillID).Add(Level, *Row);
    }

    UE_LOG(LogTemp, Warning, TEXT("Initialized %d skills"), InitializedSkills.Num());
}




bool UDESkillManagerComponent::CanLevelUp(int32 SkillID) const
{
    if (!SkillRowMap.Contains(SkillID)) return false;
    if (!HasSkillData(SkillID, GetSkillLevel(SkillID) + 1)) return false;

    // 새로 얻는 스킬이면 슬롯 여유가 있어야 한다
    if (!HasSkill(SkillID) && (!CachedInventoryComp || CachedInventoryComp->IsSkillFull())) return false;

    return true;
}

bool UDESkillManagerComponent::LevelUpSkill(int32 SkillID)
{
    if (!CanLevelUp(SkillID))
    {
        UE_LOG(LogDarkEdenSurvivors, Warning, TEXT("[Skill] Cannot level up Skill %d (Lv%d): missing row, max level, or slots full"),
            SkillID, GetSkillLevel(SkillID));
        return false;
    }

    const FDESkillRow& RowInfo = SkillRowMap[SkillID];
    const bool bHasSkill = CachedInventoryComp && CachedInventoryComp->HasSkill(SkillID);
    const int32 NewLevel = GetSkillLevel(SkillID) + 1;
    const FDESkillData* NewData = &InitializedSkills[SkillID][NewLevel];

    // --- [CASE 1: 신규 스킬 획득] ---
    if (!ActiveSkills.Contains(SkillID))
    {
        FActiveSkill NewSkill;
        NewSkill.SkillID = SkillID;
        NewSkill.RowData = NewData; // 포인터 저장
        NewSkill.CurrentCooldown = 0.f;
        NewSkill.CurrentLevel = 1;

        // 스킬 객체 생성
        if (RowInfo.SkillClass)
        {
            // 1) 객체 생성
            UDEAutoSkillBase* NewObj = NewObject<UDEAutoSkillBase>(this, RowInfo.SkillClass);

            // 2) 주인 설정 (SetOwner 대신 Base에서 만든 InitSkill 사용 권장, 없으면 SetOwner)
            NewObj->InitSkill(GetOwner());
            NewObj->SetSkillID(SkillID);
            // 3) [중요] 데이터 주입! (이제 스킬이 이 데이터를 봅니다)
            NewObj->SetSkillData(NewData);

            // 4) [가장 중요] 행동 조립! (이걸 해야 '전방 발사' 부품이 장착됨)
            NewObj->InitBehaviors();
            
            NewSkill.SkillObject = NewObj;
            UE_LOG(LogTemp, Error, TEXT("Skill ID SET : %d"), NewSkill.SkillObject->GetSkillID());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Skill] SkillRowMap has NO SkillClass for ID %d"), SkillID);
        }

        ActiveSkills.Add(SkillID, NewSkill);
        SkillLevels.Add(SkillID, 1);

        UE_LOG(LogTemp, Warning, TEXT("[Skill] NEW SKILL ACQUIRED: %d -> Lv1"), SkillID);

        // 신규 스킬이면 Inventory에 먼저 등록
        if (!bHasSkill)
        {
            CachedInventoryComp->TryAddSkill(SkillID);
            //OnSkillUpdated.Broadcast(SkillID);
        }

        return true;
    }

    // --- [CASE 2: 기존 스킬 레벨업] ---
    FActiveSkill& Active = ActiveSkills[SkillID];

    // 매니저 데이터 갱신
    Active.RowData = NewData;
    SkillLevels[SkillID] = NewLevel;
    Active.CurrentLevel = NewLevel;

    // [중요] 스킬 객체 내부 데이터도 갱신해줘야 함
    if (Active.SkillObject)
    {
        Active.SkillObject->SetSkillData(NewData);
        // (참고) 만약 레벨업할 때 스킬 메커니즘이 완전히 바뀐다면(예: 투사체->장판) 
        // 여기서 InitBehaviors()를 다시 호출해야 할 수도 있습니다. 
        // 지금은 데이터만 바뀌므로 호출 안 해도 됩니다.

        // ★ Permanent 스킬은 재발동 없이 컨텍스트만 갱신
        ESkillExecutionType Type = Active.SkillObject->GetExecutionType();
        if (Type == ESkillExecutionType::Permanent)
        {
            Active.SkillObject->RefreshContext();
        }
        else
        {
            // 즉발/지속형은 쿨타임 0으로 만들어서 다음 틱에 바로 재발동
            Active.CurrentCooldown = 0.f;
        }
    }

    OnSkillUpdated.Broadcast(SkillID);
    UE_LOG(LogTemp, Warning, TEXT("[Skill] LEVEL UP -> %d -> Lv%d"), SkillID, NewLevel);
    return true;
}

bool UDESkillManagerComponent::HasSkillData(int32 SkillID, int32 Level) const
{
    const TMap<int32, FDESkillData>* Levels = InitializedSkills.Find(SkillID);
    if (!Levels) return false;

    return Levels->Contains(Level);
}

TArray<int32> UDESkillManagerComponent::GetOwnedSkillIDs() const
{
    TArray<int32> Keys;
    SkillLevels.GetKeys(Keys);
    return Keys;
}

TArray<int32> UDESkillManagerComponent::GetUpgradeableSkills() const
{
    TArray<int32> Result;

    for (auto& Pair : SkillLevels)
    {
        int32 SkillID = Pair.Key;
        int32 CurLv = Pair.Value;

        if (HasSkillData(SkillID, CurLv + 1))
            Result.Add(SkillID);
    }

    return Result;
}

const FDESkillRow* UDESkillManagerComponent::GetRandomSkillRow()
{
    // 1. 아직 최대 레벨이 아닌 스킬 후보 수집
    TArray<int32> Candidates;

    for (auto& Pair : InitializedSkills)
    {
        int32 SkillID = Pair.Key;
        // 진화 결과 스킬은 진화로만 얻는다. 얻은 뒤의 레벨업은 일반 스킬과 같다
        const FDESkillRow* Row = GetSkillRow(SkillID);
        if (!Row || (Row->bIsEvolutionResult && !HasSkill(SkillID))) continue;

        if (CanLevelUp(SkillID))
        {
            Candidates.Add(SkillID);
        }
    }

    if (Candidates.Num() == 0)
        return nullptr;

    // 2. 랜덤 선택
    int32 Index = FMath::RandRange(0, Candidates.Num() - 1);
    int32 SelectedSkillID = Candidates[Index];

    // 3. 정체(Row) 반환
    return GetSkillRow(SelectedSkillID);
}

const FDESkillData* UDESkillManagerComponent::GetSkillDataRow(int32 SkillID, int32 Level) const
{
    // 1. 해당 SkillID의 레벨업 데이터 모음집(안쪽 맵)을 찾는다.
    if (const TMap<int32, FDESkillData>* LevelMap = InitializedSkills.Find(SkillID))
    {
        // 2. 그 안에서 정확히 요청한 Level의 데이터를 찾아 리턴한다.
        if (const FDESkillData* FoundData = LevelMap->Find(Level))
        {
            return FoundData;
        }
    }

    // 만약 만렙이라서 다음 레벨 데이터가 없거나 잘못된 ID면 nullptr 반환
    return nullptr;

}

FDESkillRow* UDESkillManagerComponent::GetSkillRow(int32 SkillID)
{
    return SkillRowMap.Find(SkillID);

}

TArray<FDESkillData*> UDESkillManagerComponent::GetRandomSkillChoices(int32 Count)
{
    TArray<FDESkillData*> Result;

    TArray<int32> Candidates;
    for (auto& Pair : InitializedSkills)
    {
        int32 SkillID = Pair.Key;
        int32 CurrentLevel = GetSkillLevel(SkillID);
        int32 NextLevel = CurrentLevel + 1;

        if (HasSkillData(SkillID, NextLevel))
            Candidates.Add(SkillID);
    }

    // --- shuffle (Fisher-Yates) ---
    for (int32 i = Candidates.Num() - 1; i > 0; --i)
    {
        int32 j = FMath::RandRange(0, i);
        Candidates.Swap(i, j);
    }
    // --------------------------------

    for (int i = 0; i < Count && i < Candidates.Num(); i++)
    {
        int32 SkillID = Candidates[i];
        int32 NextLevel = GetSkillLevel(SkillID) + 1;

        // 안전하게 참조 반환: InitializedSkills[SkillID][NextLevel]는 값 타입이므로
        Result.Add(&InitializedSkills[SkillID][NextLevel]);
    }

    return Result;
}

void UDESkillManagerComponent::ApplySkillChoice(int32 SkillID)
{
    LevelUpSkill(SkillID);
}
void UDESkillManagerComponent::LoadEvolutionDataTable()
{
    static ConstructorHelpers::FObjectFinder<UDataTable> DT_EvolutionData(
        TEXT("/Game/DarkEden/Data/Skill/DEEvolutionDataTable.DEEvolutionDataTable")
    );

    if (DT_EvolutionData.Succeeded())
    {
        EvolutionDataTable = DT_EvolutionData.Object;
        UE_LOG(LogTemp, Error, TEXT("Evolution DataTable Loaded"));
    }
    else
    {
        // 실패 시 로그 출력
        UE_LOG(LogTemp, Error, TEXT("Failed to load Evolution Data Table"));
    }
}
//bool UDESkillManagerComponent::CheckEvolution(int32& OutBaseSkillID, int32& OutResultSkillID)
bool UDESkillManagerComponent::CheckEvolution(TArray<int32>& OutConsumedSkillIDs, int32& OutResultSkillID)
{
    if (!EvolutionDataTable) return false;

    TArray<FDESkillEvolutionRow*> AllEvoRows;
    EvolutionDataTable->GetAllRows<FDESkillEvolutionRow>(TEXT("CheckEvolutionContext"), AllEvoRows);

    bool bCanEvolve = false;
    int32 HighestPriority = -1;

    // 가장 우선순위가 높은 진화식의 재료들을 임시 보관할 곳
    TArray<int32> BestConsumedSkills;

    for (FDESkillEvolutionRow* Row : AllEvoRows)
    {
        if (!Row) continue;

        bool bHasAllSkills = true;
        TArray<int32> TempConsumedSkills; // 현재 체크 중인 레시피의 재료들

        // 1. [핵심] 레시피가 요구하는 '모든' 스킬을 장착 중이며, 요구 레벨을 만족하는가?
        for (const FDESkillRequirement& ReqSkill : Row->RequiredSkills)
        {
            // 장착 안 했거나, 레벨이 딸리면 바로 이 레시피는 탈락!
            if (!ActiveSkills.Contains(ReqSkill.SkillID) || SkillLevels[ReqSkill.SkillID] < ReqSkill.RequiredLevel)
            {
                bHasAllSkills = false;
                break;
            }
            // 조건 만족 시 소모될 재료 리스트에 등록
            TempConsumedSkills.Add(ReqSkill.SkillID);
        }

        // 스킬 조건 못 채웠으면 다음 레시피로 패스
        if (!bHasAllSkills) continue;

        // 2. 악세서리 조건 체크 (기존과 동일)
        bool bHasAllAccessories = true;
        for (int32 AccID : Row->RequiredAccessoryIDs)
        {
            if (!CachedInventoryComp->HasAccessory(AccID))
            {
                bHasAllAccessories = false;
                break;
            }
        }

        if (!bHasAllAccessories) continue;

        // 3. 모든 조건을 만족했다면, 우선순위 비교!
        if (Row->Priority > HighestPriority)
        {
            HighestPriority = Row->Priority;
            BestConsumedSkills = TempConsumedSkills; // 1개일 수도, 3개일 수도 있는 재료 목록 갱신
            OutResultSkillID = Row->ResultSkillID;
            bCanEvolve = true;
        }
    }

    // 4. 최종적으로 승리한 진화/합성식의 재료 목록을 반환
    if (bCanEvolve)
    {
        OutConsumedSkillIDs = BestConsumedSkills;
    }

    return bCanEvolve;

}
//void UDESkillManagerComponent::EvolveSkill(int32 BaseSkillID, int32 ResultSkillID)
void UDESkillManagerComponent::EvolveSkill(const TArray<int32>& ConsumedSkillIDs, int32 ResultSkillID)
{
    // 1. 재료로 소모되는 '모든' 스킬을 순회하며 찢어버리기!
    for (int32 TargetSkillID : ConsumedSkillIDs)
    {
        if (ActiveSkills.Contains(TargetSkillID))
        {
            FActiveSkill& OldSkill = ActiveSkills[TargetSkillID];

            // [★핵심] 기존 스킬 객체에게 맵에 깔아둔 장판/투사체 정리 명령
            if (OldSkill.SkillObject)
            {
                // 메모리에서 안전하게 내려가도록 유도 (GC에게 맡김)
                // EndSkill만 부르면 대기 중인 파이프라인 타이머와 소유 액터가 남는다
                OldSkill.SkillObject->CancelSkill();
                OldSkill.SkillObject = nullptr;
            }

            // 매니저의 관리 목록 및 인벤토리에서 완전히 파냅니다.
            ActiveSkills.Remove(TargetSkillID);
            SkillLevels.Remove(TargetSkillID);
            CachedInventoryComp->RemoveSkill(TargetSkillID);

            UE_LOG(LogTemp, Warning, TEXT("[Evolution] Consumed SkillID: %d"), TargetSkillID);
        }
    }

    // 2. 새 진화(혹은 합성) 무기 장착! (1렙부터)
    // 아까 짜두신 '신규 스킬 획득' 함수(LevelUpSkill 등)를 그대로 재활용!
    LevelUpSkill(ResultSkillID);

    if (FActiveSkill* NewSkill = ActiveSkills.Find(ResultSkillID))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Evolution] Synthesized/Evolved into : %s successfully! (Consumed %d skills)"),
            *GetNameSafe(NewSkill->SkillObject), ConsumedSkillIDs.Num());
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("[Evolution] Failed to evolve into SkillID: %d"), ResultSkillID);
    }

}
void UDESkillManagerComponent::PauseAutoSkills()
{
    bAutoSkillPaused = true;
}

void UDESkillManagerComponent::ResumeAutoSkills()
{
    bAutoSkillPaused = false;
}

void UDESkillManagerComponent::InitStatComp(UDEStatComponent* InStatComp)
{
    CachedStatComp = InStatComp;

    if (CachedStatComp)
    {
        CachedStatComp->OnStatChanged.AddUObject(this, &UDESkillManagerComponent::OnStatChanged);
        CachedCDR = FMath::Clamp(CachedStatComp->GetStatValue(EDEStatType::Cooldown), 0.0f, 0.9f);
        UE_LOG(LogTemp, Warning, TEXT("Stat Comp Bound to Skill Manager Successfully . . . "));
    }
}

// 1. 즉시 초기화
void UDESkillManagerComponent::ResetCooldownInstant(int32 SkillID)
{
    if (FActiveSkill* FoundSkill = ActiveSkills.Find(SkillID))
    {
        FoundSkill->CurrentCooldown = 0.0f; // 0으로 밀어버림!
    }
}

// 2. 지정한 시간만큼 감소
void UDESkillManagerComponent::ReduceCooldown(int32 SkillID, float Amount, ECooldownReduceType ReduceType)
{
    if (FActiveSkill* FoundSkill = ActiveSkills.Find(SkillID))
    {
        if (!FoundSkill->RowData) return;
        float ReduceValue = 0.0f;

        switch (ReduceType)
        {
        case ECooldownReduceType::Flat:
            ReduceValue = Amount;
            break;

        case ECooldownReduceType::PercentageOfMax:
            // 1. 쿨타임 원본(혹은 플레이어 스탯이 반영된 최종 Max 쿨타임) 기준
            // (나중에 악세서리로 쿨감이 생기면 RowData->Cooldown 대신 MaxCooldown 변수를 써야 함!)
            ReduceValue = FoundSkill->RowData->Cooldown * Amount;
            break;

        case ECooldownReduceType::PercentageOfRemaining:
            // 2. 현재 째깍째깍 흘러가고 있는 남은 시간 기준
            ReduceValue = FoundSkill->CurrentCooldown * Amount;
            break;
        }
       // UE_LOG(LogTemp, Warning, TEXT("Remaining Cooldown Before Reduction : %f"),FoundSkill->CurrentCooldown);
        FoundSkill->CurrentCooldown = FMath::Max(0.0f, FoundSkill->CurrentCooldown - ReduceValue);
        TotalReducedCooldownAmount += ReduceValue;
        //UE_LOG(LogTemp, Warning, TEXT("Reduced Amount : %f / Remaining : %f"), ReduceValue, FoundSkill->CurrentCooldown);
    }

}

// 3. 전체 쿨타임 감소
void UDESkillManagerComponent::ReduceAllCooldowns(float ReduceAmount)
{
    for (auto& Pair : ActiveSkills)
    {
        FActiveSkill& Active = Pair.Value;
        Active.CurrentCooldown = FMath::Max(0.0f, Active.CurrentCooldown - ReduceAmount);
    }
}

void UDESkillManagerComponent::OnStatChanged(EDEStatType StatType, float NewValue)
{
    UE_LOG(LogTemp, Error, TEXT("On Stat Changed In Skill Manager"));
    // CDR 관련 스탯이 바뀔 때만 갱신
    if (StatType == EDEStatType::Cooldown)
    {
        CachedCDR = FMath::Clamp(NewValue, 0.0f, 0.9f);
        UE_LOG(LogTemp, Error, TEXT("CDR Changed ! : %f"), CachedCDR);
    }

    for (auto& Pair : ActiveSkills)
    {
        UDEAutoSkillBase* Skill = Pair.Value.SkillObject;
        if (!Skill || !Skill->IsRunning()) continue;

        ESkillExecutionType Type = Skill->GetExecutionType();
        if (Type == ESkillExecutionType::Permanent)
        {
            Skill->RefreshContext();
        }
    }

}
