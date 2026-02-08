// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillManagerComponent.h"
#include "DEAutoSkillBase.h"
#include "DECharacterBase.h"
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

    if (bAutoSkillPaused)
        return;

    for (auto& Pair : ActiveSkills)
    {
        FActiveSkill& Active = Pair.Value;

        // 1. 쿨타임 계산
        if (Active.CurrentCooldown > 0.f)
        {
            Active.CurrentCooldown -= DeltaTime;
            continue;
        }

        // 2. 스킬 실행
        if (Active.SkillObject && Active.RowData)
        {
            // [변경점] 인자 없이 Activate()만 호출합니다.
            // 데이터는 이미 SkillObject 내부에 저장되어 있습니다.
            Active.SkillObject->Activate();

            // 쿨타임 갱신
            Active.CurrentCooldown = Active.RowData->Cooldown;
            DeltaCheck = 0.0f;
        }
    }
    //DeltaCheck += DeltaTime;
   //Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
   //if (bAutoSkillPaused)
   //    return;
   //for (auto& Pair : ActiveSkills)
   //{
   //    FActiveSkill& Active = Pair.Value;

   //    if (Active.CurrentCooldown > 0.f)
   //    {
   //        Active.CurrentCooldown -= DeltaTime;
   //        continue;
   //    }

   //    if (Active.SkillObject && Active.RowData)
   //    {
   //        //UE_LOG(LogTemp, Warning, TEXT("%f seconds from last shot"), DeltaCheck);
   //        Active.SkillObject->ActivateSkill(Active.RowData);
   //        //UE_LOG(LogTemp, Warning, TEXT("%s : %d by Manager"), *Active.RowData->SkillName,Active.RowData->Level);
   //        Active.CurrentCooldown = Active.RowData->Cooldown; // 기본 쿨타임
   //        DeltaCheck = 0.0f;
   //    }
   //}

   //////

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




void UDESkillManagerComponent::LevelUpSkill(int32 SkillID)
{
    //if (!SkillRowMap.Contains(SkillID))
    //{
    //    UE_LOG(LogTemp, Error, TEXT("[Skill] LevelUpSkill: SkillRowMap missing SkillID %d"), SkillID);
    //    return;
    //}

    //const FDESkillRow& RowInfo = SkillRowMap[SkillID];

    //// 2) InitializedSkills 안에 이 SkillID가 있는지 확인
    //if (!InitializedSkills.Contains(SkillID))
    //{
    //    UE_LOG(LogTemp, Error, TEXT("[Skill] LevelUpSkill: InitializedSkills missing SkillID %d"), SkillID);
    //    return;
    //}

    //// 3) 현재 레벨 가져오기
    //int32 CurrentLevel = SkillLevels.Contains(SkillID) ? SkillLevels[SkillID] : 0;
    //int32 NewLevel = CurrentLevel + 1;

    //// 4) 다음 레벨 데이터가 존재하는지 확인
    //if (!InitializedSkills[SkillID].Contains(NewLevel))
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("[Skill] NO MORE LEVEL → Skill %d L%d"), SkillID, CurrentLevel);
    //    return;
    //}

    //// 5) 새로운 레벨 데이터 가져오기
    //const FDESkillData& NewData = InitializedSkills[SkillID][NewLevel];

    //// 6) 신규 스킬이면 ActiveSkills 생성
    //if (!ActiveSkills.Contains(SkillID))
    //{
    //    FActiveSkill NewSkill;
    //    NewSkill.SkillID = SkillID;
    //    NewSkill.RowData = &InitializedSkills[SkillID][NewLevel];
    //    NewSkill.CurrentCooldown = 0.f;

    //    // 스킬 오브젝트 생성
    //    if (RowInfo.SkillClass)
    //    {
    //        NewSkill.SkillObject = NewObject<UDEAutoSkillBase>(this, RowInfo.SkillClass);
    //        NewSkill.SkillObject->SetOwner(GetOwner());
    //    }
    //    else
    //    {
    //        UE_LOG(LogTemp, Error, TEXT("[Skill] SkillRowMap has NO SkillClass for ID %d"), SkillID);
    //    }

    //    ActiveSkills.Add(SkillID, NewSkill);
    //    SkillLevels.Add(SkillID, 1);

    //    UE_LOG(LogTemp, Warning, TEXT("[Skill] NEW SKILL ACQUIRED: %d → Lv1"), SkillID);
    //    return;
    //}

    //// 7) 기존 스킬이면 → 레벨업
    //FActiveSkill& Active = ActiveSkills[SkillID];
    //Active.RowData = &InitializedSkills[SkillID][NewLevel];


    //SkillLevels[SkillID] = NewLevel;

    //UE_LOG(LogTemp, Warning, TEXT("[Skill] LEVEL UP → %d → Lv%d"), SkillID, NewLevel);

    // 1. 데이터 테이블 유효성 검사 (기존 유지)
    UDEInventoryComponent* Inventory =
        GetOwner()->FindComponentByClass<UDEInventoryComponent>();

    if (!Inventory) return;

    // 신규 스킬인데 슬롯이 꽉 찼으면 컷
    const bool bHasSkill = Inventory->HasSkill(SkillID);
    if (!bHasSkill && Inventory->IsSkillFull())
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

    // 새 데이터 가져오기
    const FDESkillData* NewData = &InitializedSkills[SkillID][NewLevel];

    // --- [CASE 1: 신규 스킬 획득] ---
    if (!ActiveSkills.Contains(SkillID))
    {
        FActiveSkill NewSkill;
        NewSkill.SkillID = SkillID;
        NewSkill.RowData = NewData; // 포인터 저장
        NewSkill.CurrentCooldown = 0.f;

        // 스킬 객체 생성
        if (RowInfo.SkillClass)
        {
            // 1) 객체 생성
            UDEAutoSkillBase* NewObj = NewObject<UDEAutoSkillBase>(this, RowInfo.SkillClass);

            // 2) 주인 설정 (SetOwner 대신 Base에서 만든 InitSkill 사용 권장, 없으면 SetOwner)
            NewObj->InitSkill(GetOwner());

            // 3) [중요] 데이터 주입! (이제 스킬이 이 데이터를 봅니다)
            NewObj->SetSkillData(NewData);

            // 4) [가장 중요] 행동 조립! (이걸 해야 '전방 발사' 부품이 장착됨)
            NewObj->InitBehaviors();

            NewSkill.SkillObject = NewObj;
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
            Inventory->TryAddSkill(SkillID);
            //OnSkillUpdated.Broadcast(SkillID);
        }

        return;
    }

    // --- [CASE 2: 기존 스킬 레벨업] ---
    FActiveSkill& Active = ActiveSkills[SkillID];

    // 매니저 데이터 갱신
    Active.RowData = NewData;
    SkillLevels[SkillID] = NewLevel;

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
void UDESkillManagerComponent::PauseAutoSkills()
{
    bAutoSkillPaused = true;
}

void UDESkillManagerComponent::ResumeAutoSkills()
{
    bAutoSkillPaused = false;
}