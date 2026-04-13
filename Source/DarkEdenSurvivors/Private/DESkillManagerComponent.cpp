// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillManagerComponent.h"
#include "DEAutoSkillBase.h"
#include "DECharacterBase.h"
#include "DEStatComponent.h"
#include "DEInventoryComponent.h"
#include "Engine/World.h"

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

    // =========================================================
    // 2. 실시간 쿨타임 감소(CDR) 수치 가져오기 NOW CACHED
    // =========================================================
    //float CurrentCDR = 0.0f;
    //if (CachedStatComp)
    //{
    //    // 스탯 컴포넌트에서 실시간 쿨감 수치를 가져옵니다 (예: 0.15 = 15%)
    //    CurrentCDR = CachedStatComp->GetStatValue(EDEStatType::Cooldown);
    //}

    //// 쿨감 최대치 90% 캡(Cap) 적용 (무한 발사로 인한 게임 크래시 방지)
    //CurrentCDR = FMath::Clamp(CurrentCDR, 0.0f, 0.9f);
    
    // =========================================================
    // 3. 스킬 루프 및 발동
    // =========================================================
    for (auto& Pair : ActiveSkills)
    {
        FActiveSkill& Active = Pair.Value;

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

            //// 데미지 합산, 스냅샷 등 복잡한 로직은 스킬 내부(BuildContext)가 알아서 함!
            //// 매니저는 그냥 "쏴라!" 하고 명령만 내립니다.
            //Active.SkillObject->Activate();

            //// ---------------------------------------------------------
            //// 4. 다음 발사를 위한 쿨타임 리셋 (Over-tick 최적화 포함)
            //// ---------------------------------------------------------
            //float BaseCooldown = Active.RowData->Cooldown;

            //// 기본 쿨타임에 현재 쿨감(%)을 적용 (최소 0.1초 방어선)
            //float FinalCooldown = FMath::Max(0.1f, BaseCooldown * (1.0f - CurrentCDR));
            //// 초과 시간(Over-tick) 보정: 프레임 랙 때문에 쿨타임이 -0.05초가 되었다면,
            //// 다음 쿨타임에서 0.05초를 빼서 스킬 템포를 칼같이 유지합니다.
            //Active.CurrentCooldown = FinalCooldown + Active.CurrentCooldown;

            //// 만약 초과 보정 때문에 쿨이 또 0 이하로 내려가면 안전하게 리셋
            //if (Active.CurrentCooldown <= 0.0f)
            //{
            //    Active.CurrentCooldown = FinalCooldown;
            //}
        }
    }

    ////DeltaCheck += DeltaTime;
    //Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //if (bAutoSkillPaused)
    //    return;

    //for (auto& Pair : ActiveSkills)
    //{
    //    FActiveSkill& Active = Pair.Value;

    //    // 1. 쿨타임 계산
    //    if (Active.CurrentCooldown > 0.f)
    //    {
    //        Active.CurrentCooldown -= DeltaTime;
    //        continue;
    //    }

    //    // 2. 스킬 실행
    //    if (Active.SkillObject && Active.RowData)
    //    {
    //        // [변경점] 인자 없이 Activate()만 호출합니다.
    //        // 데이터는 이미 SkillObject 내부에 저장되어 있습니다.
    //        Active.SkillObject->Activate();

    //        // 쿨타임 갱신
    //        Active.CurrentCooldown = Active.RowData->Cooldown;
    //        //DeltaCheck = 0.0f;
    //    }
    //}


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




void UDESkillManagerComponent::LevelUpSkill(int32 SkillID)
{
    UE_LOG(LogTemp, Warning, TEXT("Try Level Up Skill for : %d"),SkillID);
    // 1. 데이터 테이블 유효성 검사 (기존 유지)
    if (!CachedInventoryComp) return;

    // 신규 스킬인데 슬롯이 꽉 찼으면 컷
    const bool bHasSkill = CachedInventoryComp->HasSkill(SkillID);
    if (!bHasSkill && CachedInventoryComp->IsSkillFull())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Skill] Cannot acquire Skill %d: Skill slots full"),
            SkillID);
        return;
    }

  

    if (!SkillRowMap.Contains(SkillID))
    {
        UE_LOG(LogTemp, Error, TEXT("[Skill] LevelUpSkill: SkillRowMap missing SkillID %d"), SkillID);
        return;
    }

    const FDESkillRow& RowInfo = SkillRowMap[SkillID];

    if (!InitializedSkills.Contains(SkillID))
    {
        UE_LOG(LogTemp, Error, TEXT("[Skill] LevelUpSkill: InitializedSkills missing SkillID %d"), SkillID);
        return;
    }

    // 2. 레벨 계산
    int32 CurrentLevel = SkillLevels.Contains(SkillID) ? SkillLevels[SkillID] : 0;
    int32 NewLevel = CurrentLevel + 1;

    // 만렙 체크
    if (!InitializedSkills[SkillID].Contains(NewLevel))
    {
        UE_LOG(LogTemp, Warning, TEXT("[Skill] NO MORE LEVEL -> Skill %d L%d"), SkillID, CurrentLevel);
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Skill Level Up Passed All Criteria"));
    // 새 데이터 가져오기
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

        return;
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
    }

    OnSkillUpdated.Broadcast(SkillID);
    UE_LOG(LogTemp, Warning, TEXT("[Skill] LEVEL UP -> %d -> Lv%d"), SkillID, NewLevel);

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
        //check if evol
        const FDESkillRow* Row = GetSkillRow(SkillID);
        if (!Row || Row->bIsEvolutionResult) continue;

        //check if max lvl
        int32 CurrentLevel = GetSkillLevel(SkillID);
        int32 NextLevel = CurrentLevel + 1;

        if (HasSkillData(SkillID, NextLevel))
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
bool UDESkillManagerComponent::CheckEvolution(int32& OutBaseSkillID, int32& OutResultSkillID)
{
    UE_LOG(LogTemp, Warning, TEXT("[Evolution] Try Check Evolution . . ."));
    if (!EvolutionDataTable) return false;
    UE_LOG(LogTemp, Warning, TEXT("[Evolution] Passed DataTable Check . . ."));
    // 데이터 테이블의 모든 행 가져오기
    TArray<FDESkillEvolutionRow*> AllEvoRows;
    EvolutionDataTable->GetAllRows<FDESkillEvolutionRow>(TEXT("CheckEvolutionContext"), AllEvoRows);

    bool bCanEvolve = false;
    int32 HighestPriority = -1; // 우선순위 비교용

    for (FDESkillEvolutionRow* Row : AllEvoRows)
    {
        if (!Row) continue;

        // 1. 기본 무기를 장착 중인가?
        if (!ActiveSkills.Contains(Row->BaseSkillID)) continue;

        // 2. 그 무기가 요구 레벨을 달성했는가?
        if (SkillLevels[Row->BaseSkillID] < Row->RequiredSkillLevel) continue;

        UE_LOG(LogTemp, Warning, TEXT("[Evolution] %d has MAX LEVEL to Evolve . . ."), SkillLevels[Row->BaseSkillID]);

        bool bHasAllAccessories = true;
        for (int32 AccID : Row->RequiredAccessoryIDs)
        {
            if (!CachedInventoryComp->HasAccessory(AccID))
            {
                bHasAllAccessories = false;
                UE_LOG(LogTemp, Warning, TEXT("[Evolution] %d has MAX LEVEL but No Acc . . ."), SkillLevels[Row->BaseSkillID]);
                break; // 하나라도 없으면 이 레시피는 탈락!
            }
        }
        // 악세서리가 부족하면 다음 레시피로 패스
        if (!bHasAllAccessories) continue;

        // 4. ★ 모든 조건을 만족했다면, 여태 찾은 진화식보다 우선순위가 높은지 체크!
        if (Row->Priority > HighestPriority)
        {
            HighestPriority = Row->Priority;
            OutBaseSkillID = Row->BaseSkillID;
            OutResultSkillID = Row->ResultSkillID;
            bCanEvolve = true;
            UE_LOG(LogTemp, Warning, TEXT("[Evolution] Can Evolve !"));
        }
    }
    
    return bCanEvolve;
}
void UDESkillManagerComponent::EvolveSkill(int32 BaseSkillID, int32 ResultSkillID)
{
    // 1. 기존 스킬 찢어버리기!
    if (ActiveSkills.Contains(BaseSkillID))
    {
        FActiveSkill& OldSkill = ActiveSkills[BaseSkillID];

        // [★핵심] 기존 스킬 객체에게 "너 이제 해고니까, 네가 맵에 깔아둔 장판/투사체 싹 다 지워!" 라고 명령
        if (OldSkill.SkillObject)
        {
            // 이 함수는 UDESkillBase나 AutoSkillBase에 하나 파두셔야 합니다.
            // (내부에서 풀링된 투사체들을 회수하거나 Destroy 하도록 구현)
            // OldSkill.SkillObject->DestroySkill(); 

            // 메모리에서 안전하게 내려가도록 유도 (GC에게 맡김)
            OldSkill.SkillObject->EndSkill(); // 한 줄로 끝
            OldSkill.SkillObject = nullptr;
        }

        // 매니저의 관리 목록에서 완전히 파냅니다.
        ActiveSkills.Remove(BaseSkillID);
        SkillLevels.Remove(BaseSkillID);
        CachedInventoryComp->RemoveSkill(BaseSkillID);
    }

    // 2. 새 진화 무기 장착! (1렙부터)
    // 아까 짜두신 '신규 스킬 획득' 함수(LevelUpSkill 등)를 그대로 재활용!
    LevelUpSkill(ResultSkillID);

    if (FActiveSkill* NewSkill = ActiveSkills.Find(ResultSkillID))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[Evolution] Evolved into : %s successfully"),
            *NewSkill->SkillObject->GetName());
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

    //if (FActiveSkill* FoundSkill = ActiveSkills.Find(SkillID))
    //{
    //    // 스킬 데이터가 없으면 리턴
    //    if (!FoundSkill->RowData) return;

    //    float ReduceValue = 0.0f;

    //    // 1. 타입에 따른 감소량(초) 계산
    //    switch (ReduceType)
    //    {
    //    case ECooldownReduceType::Flat:
    //        // 고정 시간 (예: Amount가 1.5면 1.5초 감소)
    //        ReduceValue = Amount;
    //        break;

    //    case ECooldownReduceType::Percentage:
    //        // 퍼센트 (예: Amount가 0.2면 최대 쿨타임의 20% 감소)
    //        // BaseCooldown을 기준으로 깎아야 기획 의도에 맞음!
    //        float BaseCooldown = FoundSkill->RowData->Cooldown;
    //        ReduceValue = BaseCooldown * Amount;
    //        break;
    //    }

    //    // 2. 남은 쿨타임에서 차감 (0 이하로 내려가지 않게 방어)
    //    FoundSkill->CurrentCooldown = FMath::Max(0.0f, FoundSkill->CurrentCooldown - ReduceValue);
    //    UE_LOG(LogTemp, Warning, TEXT("Cooldown Reduced ! Reduced Amount : %f"), ReduceValue);
    //}
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
    // CDR 관련 스탯이 바뀔 때만 갱신
    if (StatType == EDEStatType::Cooldown)
    {
        CachedCDR = FMath::Clamp(NewValue, 0.0f, 0.9f);
        UE_LOG(LogTemp, Error, TEXT("CDR Changed ! : %f"), CachedCDR);
    }
}
