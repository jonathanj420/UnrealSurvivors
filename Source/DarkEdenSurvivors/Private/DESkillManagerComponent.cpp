// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillManagerComponent.h"
#include "DESkillBase.h"
#include "DESkillBloodyKnife.h"
#include "DECharacterBase.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UDESkillManagerComponent::UDESkillManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
    LoadSkillRowTable();
    LoadSkillDataTable();
   

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
    DeltaCheck += DeltaTime;
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    for (auto& Pair : ActiveSkills)
    {
        FActiveSkill& Active = Pair.Value;

        if (Active.CurrentCooldown > 0.f)
        {
            Active.CurrentCooldown -= DeltaTime;
            continue;
        }

        if (Active.SkillObject && Active.RowData)
        {
            //UE_LOG(LogTemp, Warning, TEXT("%f seconds from last shot"), DeltaCheck);
            Active.SkillObject->ActivateSkill(Active.RowData);
            //UE_LOG(LogTemp, Warning, TEXT("%s : %d by Manager"), *Active.RowData->SkillName,Active.RowData->Level);
            Active.CurrentCooldown = Active.RowData->Cooldown; // 기본 쿨타임
            DeltaCheck = 0.0f;
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
        FDESkillData* Row = SkillDataTable->FindRow<FDESkillData>(RowName, Context);
        UE_LOG(LogTemp, Warning, TEXT("Row %s_%d Found"), *Row->SkillName,Row->Level);
        if (!Row) continue;

        int32 SkillID = Row->SkillID;   // RowName 사용 안 하는 방식
        int32 Level = Row->Level;

        InitializedSkills.FindOrAdd(SkillID).Add(Level, *Row);
    }

    UE_LOG(LogTemp, Warning, TEXT("Initialized %d skills"), InitializedSkills.Num());
}

//FSkillSpec UDESkillManagerComponent::MakeSkillSpec(int32 SkillID)
//{
//
//    FSkillSpec Spec;
//
//    const TMap<int32, FDESkillData>* Levels = InitializedSkills.Find(SkillID);
//    if (!Levels) return Spec;
//
//    int32 Level = SkillLevels.Contains(SkillID) ? SkillLevels[SkillID] : 1;
//    const FDESkillData* Row = Levels->Find(Level);
//    if (!Row) return Spec;
//
//    Spec.Level = Level;
//    Spec.Damage = Row->Damage;
//    Spec.ProjectileCount = Row->ProjectileCount;
//    Spec.Penetration = Row->Penetration;
//    Spec.Cooldown = Row->Cooldown;
//    Spec.SkillActorClass = Row->SkillActorClass;
//    // (필요하면 Spec.SkillClass/SkillName 등 추가)
//
//    return Spec;
//
//}

void UDESkillManagerComponent::AddSkill(int32 SkillID)
{
    //if (!InitializedSkills.Contains(SkillID))
    //    return;

    //// 이미 보유한 스킬이면 무시
    //if (SkillLevels.Contains(SkillID))
    //    return;

    //SkillLevels.Add(SkillID, 1);

    //FSkillSpec Spec = MakeSkillSpec(SkillID);
    //if (!Spec.SkillActorClass) return;

    //// 스킬 객체 생성
    //FDESkillData* Row = nullptr;
    //for (FDESkillData* R : InitializedSkills[SkillID])
    //    if (R->Level == 1) Row = R;

    //if (!Row || !Row->SkillClass) return;

    //UDESkillBase* SkillObj = NewObject<UDESkillBase>(this, Row->SkillClass);
    //SkillObj->SetOwner(GetOwner());

    //FActiveSkill NewSkill;
    //NewSkill.SkillObject = SkillObj;
    //NewSkill.Spec = Spec;

    //ActiveSkills.Add(SkillID, NewSkill);

    //UE_LOG(LogTemp, Warning, TEXT("Added New Skill %d (Lv1)"), SkillID);
}


void UDESkillManagerComponent::LevelUpSkill(int32 SkillID)
{
    //int32 NewLevel = 1;

    //// 1) 스킬이 없다 = 신규 스킬
    //if (!SkillLevels.Contains(SkillID))
    //{
    //    SkillLevels.Add(SkillID, 1);
    //    NewLevel = 1;
    //    UE_LOG(LogTemp, Warning, TEXT("[Skill] New Skill Acquired: %d (Lv1)"), SkillID);
    //}
    //else
    //{
    //    // 2) 기존 스킬 → 레벨업
    //    int32 CurLv = SkillLevels[SkillID];
    //    int32 NextLv = CurLv + 1;

    //    if (!HasSkillData(SkillID, NextLv))
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("[Skill] No more upgrades for Skill %d (Lv%d)"), SkillID, CurLv);
    //        return;
    //    }

    //    SkillLevels[SkillID] = NextLv;
    //    NewLevel = NextLv;
    //    UE_LOG(LogTemp, Warning, TEXT("[Skill] Level Up → Skill %d (Lv%d)"), SkillID, NewLevel);
    //}

    //// 새 레벨 데이터 가져오기
    //FDESkillData* NewRow = nullptr;
    //for (auto& RowName : SkillDataTable->GetRowNames())
    //{
    //    FDESkillData* TempRow = SkillDataTable->FindRow<FDESkillData>(RowName, TEXT(""));
    //    if (TempRow && TempRow->SkillID == SkillID && TempRow->Level == NewLevel)
    //    {
    //        NewRow = TempRow;
    //        break;
    //    }
    //}

    //if (!NewRow)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("[Skill] LevelUpSkill ERROR — No FDESkillData for SkillID %d, Level %d"), SkillID, NewLevel);
    //    return;
    //}

    //// ActiveSkill이 이미 존재하면 RowData만 갱신
    //if (ActiveSkills.Contains(SkillID))
    //{
    //    ActiveSkills[SkillID].RowData = NewRow;
    //    UE_LOG(LogTemp, Warning, TEXT("[Skill] ActiveSkill Updated for Skill %d (Lv%d)"), SkillID, NewLevel);
    //}
    //else
    //{
    //    // 신규 ActiveSkill 생성
    //    if (!NewRow->SkillClass)
    //    {
    //        UE_LOG(LogTemp, Error, TEXT("[Skill] LevelUpSkill ERROR — No SkillClass for SkillID %d"), SkillID);
    //        return;
    //    }

    //    UDESkillBase* SkillObj = NewObject<UDESkillBase>(this, NewRow->SkillClass);
    //    SkillObj->SetOwner(GetOwner());

    //    FActiveSkill NewActive;
    //    NewActive.SkillObject = SkillObj;
    //    NewActive.RowData = NewRow;
    //    NewActive.CurrentCooldown = 0.f;

    //    ActiveSkills.Add(SkillID, NewActive);

    //    UE_LOG(LogTemp, Warning, TEXT("[Skill] ActiveSkill Created for Skill %d"), SkillID);
    //}
    // 1) SkillRowMap에서 스킬 존재 확인 (클래스/이름)
    // 1) SkillRowMap 존재 확인 (클래스, 이름 등)
    if (!SkillRowMap.Contains(SkillID))
    {
        UE_LOG(LogTemp, Error, TEXT("[Skill] LevelUpSkill: SkillRowMap missing SkillID %d"), SkillID);
        return;
    }

    const FDESkillRow& RowInfo = SkillRowMap[SkillID];

    // 2) InitializedSkills 안에 이 SkillID가 있는지 확인
    if (!InitializedSkills.Contains(SkillID))
    {
        UE_LOG(LogTemp, Error, TEXT("[Skill] LevelUpSkill: InitializedSkills missing SkillID %d"), SkillID);
        return;
    }

    // 3) 현재 레벨 가져오기
    int32 CurrentLevel = SkillLevels.Contains(SkillID) ? SkillLevels[SkillID] : 0;
    int32 NewLevel = CurrentLevel + 1;

    // 4) 다음 레벨 데이터가 존재하는지 확인
    if (!InitializedSkills[SkillID].Contains(NewLevel))
    {
        UE_LOG(LogTemp, Warning, TEXT("[Skill] NO MORE LEVEL → Skill %d L%d"), SkillID, CurrentLevel);
        return;
    }

    // 5) 새로운 레벨 데이터 가져오기
    const FDESkillData& NewData = InitializedSkills[SkillID][NewLevel];

    // 6) 신규 스킬이면 ActiveSkills 생성
    if (!ActiveSkills.Contains(SkillID))
    {
        FActiveSkill NewSkill;
        NewSkill.SkillID = SkillID;
        NewSkill.RowData = &InitializedSkills[SkillID][NewLevel];
        NewSkill.CurrentCooldown = 0.f;

        // 스킬 오브젝트 생성
        if (RowInfo.SkillClass)
        {
            NewSkill.SkillObject = NewObject<UDESkillBase>(this, RowInfo.SkillClass);
            NewSkill.SkillObject->SetOwner(GetOwner());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[Skill] SkillRowMap has NO SkillClass for ID %d"), SkillID);
        }

        ActiveSkills.Add(SkillID, NewSkill);
        SkillLevels.Add(SkillID, 1);

        UE_LOG(LogTemp, Warning, TEXT("[Skill] NEW SKILL ACQUIRED: %d → Lv1"), SkillID);
        return;
    }

    // 7) 기존 스킬이면 → 레벨업
    FActiveSkill& Active = ActiveSkills[SkillID];
    Active.RowData = &InitializedSkills[SkillID][NewLevel];


    SkillLevels[SkillID] = NewLevel;

    UE_LOG(LogTemp, Warning, TEXT("[Skill] LEVEL UP → %d → Lv%d"), SkillID, NewLevel);
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
