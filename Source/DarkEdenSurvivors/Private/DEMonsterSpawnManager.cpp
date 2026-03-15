// Fill out your copyright notice in the Description page of Project Settings.


#include "DEMonsterSpawnManager.h"
#include "DEMonsterBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DECharacterBase.h"
#include "DEGameInstance.h"
#include "DEGameMode_Stage.h"
#include "DEMonsterUpdateComponent.h"
#include "DEPickupManager.h"

ADEMonsterSpawnManager::ADEMonsterSpawnManager()
{
    PrimaryActorTick.bCanEverTick = true;
    MonsterBase = ADEMonsterBase::StaticClass();
    CurrentWaveIndex = -1;
    WaveElapsedTime = 0.0f;
    NextSpawnTime = 0.0f;


    USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootSceneComponent;
    MonsterUpdateComponent = CreateDefaultSubobject<UDEMonsterUpdateComponent>(TEXT("MonsterUpdater"));
}

void ADEMonsterSpawnManager::BeginPlay()
{
    Super::BeginPlay();

    GameInstanceCache = Cast<UDEGameInstance>(GetGameInstance());

    if (!GameInstanceCache)
    {
        UE_LOG(LogTemp, Error, TEXT("MonsterSpawnManager: Failed to cache GameInstance!"));
    }

    GameMode = Cast<ADEGameMode_Stage>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Mode CHECKED"));
        GameMode->RegisterMonsterSpawnManager(this);
    }
    

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (PlayerController)
    {
        APawn* PlayerPawn = PlayerController->GetPawn();

        Player = Cast<ADECharacterBase>(PlayerPawn);
        MonsterUpdateComponent->SetPlayer(Player);

    }
    // StageWaveTable이 있으면 RowName을 읽어와 StartTime 기준으로 정렬해서 캐시함
    StageRowNames.Empty();
    if (StageWaveTable)
    {
        struct FRowPair { FName Name; float Start; };
        TArray<FRowPair> Pairs;

        TArray<FName> AllNames = StageWaveTable->GetRowNames();
        for (FName RN : AllNames)
        {
            const FDEStageWaveData* Row = StageWaveTable->FindRow<FDEStageWaveData>(RN, TEXT(""));
            if (Row)
            {
                Pairs.Add({ RN, Row->StartTime });
            }
        }

        // StartTime 오름차순 정렬
        Pairs.Sort([](const FRowPair& A, const FRowPair& B) {
            return A.Start < B.Start;
            });

        for (const FRowPair& P : Pairs)
            StageRowNames.Add(P.Name);

        UE_LOG(LogTemp, Warning, TEXT("StageWaveTable loaded: %d rows"), StageRowNames.Num());
    }

    // 초기화
    CurrentWaveIndex = -1;
    WaveElapsedTime = 0.0f;
    NextSpawnTime = 0.0f;
    /*InitializePool();

    StartNextWave();*/

}

void ADEMonsterSpawnManager::Tick(float DeltaTime)
{
    //control all monsters' logic here
    Super::Tick(DeltaTime);

    // 현재 월드 시간 (한 번만 가져와서 1000마리한테 돌려씀 -> 최적화)
    //double CurrentWorldTime = GetWorld()->GetTimeSeconds();

    if (!Player) return;
    ProcessWave(DeltaTime);

    MonsterUpdateComponent->UpdateMonsters(DeltaTime, ActiveMonsters);


}

//************************NEW FUNC
void ADEMonsterSpawnManager::ProcessWave(float DeltaTime)
{
    if (!GameMode || StageRowNames.Num() == 0)
        return;

    const float Elapsed = GameMode->GetElapsedTime();


    while (CurrentWaveIndex + 1 < StageRowNames.Num())
    {
        int32 NextIndex = CurrentWaveIndex + 1;
        FName NextName = StageRowNames[NextIndex];
        const FDEStageWaveData* NextRow = StageWaveTable->FindRow<FDEStageWaveData>(NextName, TEXT(""));

        if (NextRow && Elapsed >= NextRow->StartTime)
        {
            CurrentWaveIndex = NextIndex;
            //UE_LOG(LogTemp, Warning, TEXT("Starting wave for Current Index : %d"), CurrentWaveIndex);
            StartWave(CurrentWaveIndex);
        }
        else
        {
            break; // ← 이게 있어야 함
        }
    }


    // 현재 활성 웨이브가 없다면 종료
    if (CurrentWaveIndex < 0 || CurrentWaveIndex >= StageRowNames.Num())
        return;

    // 현재 웨이브 데이터 가져오기
    FName CurName = StageRowNames[CurrentWaveIndex];
    const FDEStageWaveData* CurRow = StageWaveTable->FindRow<FDEStageWaveData>(CurName, TEXT(""));
    if (!CurRow) return;

    // 누적 시간 업데이트
    WaveElapsedTime += DeltaTime;

    //// Duration이 0이면 무한(또는 단발 보스만)으로 간주 — Duration > 0 이면 그 기간까지만 Interval 스폰
    //if (CurRow->Duration > 0.0f && WaveElapsedTime >= CurRow->Duration)
    //{
    //    // 웨이브 시간 끝 — 다음 웨이브는 StartTime 기준으로 대기
    //    return;
    //}
    // =================================================================
    // [수정] Duration 규칙 적용
    // =================================================================

    // 1. Duration이 0이다? -> "단발성(One-Shot)" 웨이브.
    //    StartWave에서 MinimumCount만큼 이미 뽑았으므로, 추가 스폰(Interval)은 절대 하지 않음.
    if (FMath::IsNearlyZero(CurRow->Duration))
    {
        return;
    }

    // 2. Duration이 양수(>0)다? -> "시간제한(Timed)" 웨이브. 
    //    시간이 다 되면 스폰을 멈춤 (다음 웨이브 대기 상태로 진입).
    if (CurRow->Duration > 0.0f && WaveElapsedTime >= CurRow->Duration)
    {
        return;
    }

    // 3. Duration이 음수(-1)다? -> "무한(Infinite)" 웨이브.
    //    위의 조건들에 걸리지 않으므로 계속 아래로 내려가서 스폰함.

    // =================================================================


    // SpawnInterval 기준으로 주기 스폰
    // NextSpawnTime은 절대 시간(게임 전체 경과 기준)
    const float Now = GameMode->GetElapsedTime();
    if (Now >= NextSpawnTime)
    {
        bool bSpawned = TrySpawnMonster(*CurRow);
        if (bSpawned)
        {
            NextSpawnTime = Now + FMath::Max(0.01f, CurRow->SpawnInterval);
        }
        else
        {
            // 스폰 실패(풀 exhausted / spawnlimit)면 약간 지연해서 재시도
            NextSpawnTime = Now + 0.2f;
        }
    }
}

void ADEMonsterSpawnManager::StartWave(int32 WaveIndex)
{


    if (WaveIndex < 0 || WaveIndex >= StageRowNames.Num())
        return;

    FName RowName = StageRowNames[WaveIndex];
    const FDEStageWaveData* WaveData = StageWaveTable->FindRow<FDEStageWaveData>(RowName, TEXT(""));
    if (!WaveData) return;

    WaveElapsedTime = 0.0f;

    //UE_LOG(LogTemp, Warning, TEXT("StartWave %d (StartTime %.2f)"), WaveIndex, WaveData->StartTime);

    // 보스 즉시 스폰 (보스가 지정되어 있으면 한 번만)
    if (!WaveData->BossMonsterID.IsNone())
    {
        SpawnBoss(*WaveData);
    }

    for (auto MobName : WaveData->SpawnMonsterIDs)
    {
       // UE_LOG(LogTemp, Warning, TEXT("Monster Found : %s"), *MobName.ToString());
    }
    // 2. MinimumCount 즉시 보충 (SpawnLimit 고려)
    int32 CurrentActive = ActiveMonsters.Num();
    int32 SpawnLimit = GameMode ? GameMode->GetSpawnLimit() : INT32_MAX;

    //  [수정 전] 전체 몹 수량에서 빼버리는 멍청한 짓
    // int32 Need = FMath::Max(0, WaveData->MinimumCount - CurrentActive);

    //  [수정 후] 이 웨이브가 시작할 때 무조건 뿌려야 하는 고유 할당량!
    int32 Need = WaveData->MinimumCount;

    // 엔진 터지는 건 막아야 하니, '맵 전체의 남은 빈자리(CanSpawn)' 계산
    int32 CanSpawn = FMath::Max(0, SpawnLimit - CurrentActive);

    // 할당량과 빈자리 중 더 작은 값만큼 최종 소환!
    int32 ToSpawn = FMath::Min(Need, CanSpawn);

    for (int32 i = 0; i < ToSpawn; ++i)
    {
        // [수정] 클래스 배열 대신 ID 배열(SpawnMonsterIDs) 체크
        if (WaveData->SpawnMonsterIDs.Num() == 0) break;

        // A. 랜덤 ID 뽑기
        int32 RandIdx = FMath::RandRange(0, WaveData->SpawnMonsterIDs.Num() - 1);
        FName TargetID = WaveData->SpawnMonsterIDs[RandIdx];
       // UE_LOG(LogTemp, Warning, TEXT("Target Monster Name : %s"),*TargetID.ToString());
        // B. GameInstance에서 데이터 포인터 가져오기 (O(1) 속도)
        const FDEMonsterData* MonsterData = GameInstanceCache->GetMonsterData(TargetID);

        // C. 데이터가 유효하면 스폰 (데이터 주입)
        if (MonsterData)
        {
            FVector SpawnLocation = GetRandomSpawnLocation();

            // SpawnFromPool(위치, 데이터포인터) 호출
            SpawnFromPool(SpawnLocation, MonsterData);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("StartWave: Monster Data Not Found for ID '%s'"), *TargetID.ToString());
        }
    }

    // 다음 스폰 시간 초기화
    // [수정 전] 지금 당장 또 뽑아라 (X)
    // NextSpawnTime = GameMode ? GameMode->GetElapsedTime() : 0.0f;

    // [수정 후] "인터벌 시간 뒤"에 첫 주기적 스폰을 시작해라 (O)
    float CurrentTime = GameMode ? GameMode->GetElapsedTime() : 0.0f;

    // WaveData의 SpawnInterval만큼 뒤로 미룸
    // (Interval이 0이면 0.01f라도 더해서 즉시 실행 방지)
    float FirstDelay = FMath::Max(0.01f, WaveData->SpawnInterval);

    NextSpawnTime = CurrentTime + FirstDelay;
    
}

bool ADEMonsterSpawnManager::TrySpawnMonster(const FDEStageWaveData& WaveData)
{

   // UE_LOG(LogTemp, Error, TEXT("Try to Spawn Monster"));
    // A. 스폰 제한 체크
    const int32 CurrentActiveCount = ActiveMonsters.Num();
    const int32 SpawnLimit = GameMode ? GameMode->GetSpawnLimit() : 500;

    if (CurrentActiveCount >= SpawnLimit) return false;
    if (WaveData.SpawnMonsterIDs.Num() == 0) return false;

    // B. 이번에 소환할 몬스터 ID 랜덤 선택
    int32 RandIdx = FMath::RandRange(0, WaveData.SpawnMonsterIDs.Num() - 1);
    FName TargetID = WaveData.SpawnMonsterIDs[RandIdx];

    // C. GameInstance에서 캐싱된 데이터 가져오기 (초고속 조회)
    const FDEMonsterData* MonsterData = GameInstanceCache->GetMonsterData(TargetID);
    if (!MonsterData)
    {
        // 오타나서 데이터 없으면 로그 띄우고 패스
        UE_LOG(LogTemp, Error, TEXT("Spawn Failed: Data Not Found for ID '%s'"), *TargetID.ToString());
        return false;
    }

    // D. 위치 잡고 스폰 실행 (데이터 전달)
    FVector SpawnLocation = GetRandomSpawnLocation();

    // 더 이상 MonsterClass를 넘기지 않음 (MasterClass 사용)
    SpawnFromPool(SpawnLocation, MonsterData);

    return true;
}

bool ADEMonsterSpawnManager::SpawnBoss(const FDEStageWaveData& WaveData)
{
    // 1. 유효성 체크
    if (WaveData.BossMonsterID.IsNone() || !GameInstanceCache)
        return false;

    // 2. 데이터 가져오기
    const FDEMonsterData* BossData = GameInstanceCache->GetMonsterData(WaveData.BossMonsterID);
    if (!BossData)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnBoss Failed: Data Not Found (%s)"), *WaveData.BossMonsterID.ToString());
        return false;
    }

    // 3. 보스 전용 클래스 확인 (OverrideClass)
    // 보스는 무조건 데이터 테이블에 전용 BP(OverrideClass)가 지정되어 있어야 함!
    TSubclassOf<ADEMonsterBase> BossClassToSpawn;

    if (!BossData->OverrideClass.IsNull())
    {
        // 보스는 한 마리니까 여기서 즉시 로딩(Synchronous Load)해도 괜찮음
        BossClassToSpawn = BossData->OverrideClass.LoadSynchronous();
    }

    if (!BossClassToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnBoss Failed: No OverrideClass in Data (%s). Boss needs a specific BP!"), *WaveData.BossMonsterID.ToString());
        return false;
    }

    // 4. 스폰 실행 (풀링 X, 직접 생성)
    FVector SpawnLocation = GetRandomSpawnLocation(); // 나중엔 보스 전용 스폰 위치로 변경 가능

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 보스는 껴서라도 나와야 함

    ADEMonsterBase* SpawnedBoss = GetWorld()->SpawnActor<ADEMonsterBase>(
        BossClassToSpawn,
        SpawnLocation,
        FRotator::ZeroRotator,
        Params
    );

    // 5. 초기화 및 연출 시작
    if (SpawnedBoss)
    {
        // 데이터(체력 등) 적용
        SpawnedBoss->ResetMonster(BossData);

        // [연출 포인트] 여기서 보스 등장 로직 실행!
        // 예: 카메라 쉐이크, 보스 이름 UI 띄우기, 포효 애니메이션 등
        // AADEBossMonster* RealBoss = Cast<AADEBossMonster>(SpawnedBoss);
        // if (RealBoss) RealBoss->PlayIntroSequence();

        UE_LOG(LogTemp, Warning, TEXT("!!! BOSS SPAWNED: %s !!!"), *BossData->DisplayName.ToString());

        // 보스는 ActiveMonsters에 넣을지 말지 선택 (일반 몹 카운트에 포함시킬지?)
        // 보통 보스는 별도 관리하거나 포함시키더라도 Limit 체크에서 예외 처리함
        ActiveMonsters.Add(SpawnedBoss);

        return true;
    }

    return false;
}


ADEMonsterBase* ADEMonsterSpawnManager::SpawnFromPool(FVector& Location, const FDEMonsterData* DataToApply)
{

    // --------------------------------------------------------
    // 1. [Class 결정] 기본 몹이냐? 데이터에 적힌 특수 몹(보스)이냐?
    // --------------------------------------------------------
    TSubclassOf<ADEMonsterBase> ClassToSpawn = MonsterBase; // 기본값

    if (DataToApply && !DataToApply->OverrideClass.IsNull())
    {
        ClassToSpawn = DataToApply->OverrideClass.LoadSynchronous();
    }

    if (!ClassToSpawn) return nullptr;

    // --------------------------------------------------------
    // 2. [높이 보정] CDO를 사용해 "자동으로" 땅에 안 박히게 만들기
    // --------------------------------------------------------
    // CDO(Class Default Object): 블루프린트의 기본 세팅값을 읽어옴
    const ADEMonsterBase* CDO = ClassToSpawn.GetDefaultObject();
    if (CDO)
    {
        // 땅바닥 위치(Location.Z) + 캡슐 절반 높이 = 발바닥이 땅에 닿음
        float HalfHeight = CDO->GetCapsuleHalfHeight();
        Location.Z += HalfHeight;
    }

    // --------------------------------------------------------
    // 3. [Pooling] 풀 뒤지기 (클래스 타입 검사 필수!)
    // --------------------------------------------------------
    ADEMonsterBase* SpawnedMonster = nullptr;
   // UE_LOG(LogTemp, Warning, TEXT("Try Spawn From Pool First"));
    // *InactiveMonsters: 죽어서 대기 중인 몬스터 목록이라고 가정
    for (int32 i = 0; i < InactiveMonsters.Num(); i++)
    {
        ADEMonsterBase* Candidate = InactiveMonsters[i];

        // ★ [핵심 보완] 
        // 1. 유효한가?
        // 2. 내가 찾는 클래스(ClassToSpawn)와 같은 종류인가? (좀비 찾는데 스켈레톤 꺼내면 안됨)
        if (Candidate && Candidate->GetClass() == ClassToSpawn)
        {
            SpawnedMonster = Candidate;
            //InactiveMonsters.RemoveAt(i); // 대기열에서 제외
            // RemoveAt 대신 RemoveAtSwap 사용 (O(N) -> O(1))
            InactiveMonsters.RemoveAtSwap(i);
          //  UE_LOG(LogTemp, Warning, TEXT("Monster ReSpawned From Pool"));
            break;
        }
    }

    // --------------------------------------------------------
    // 4. [Spawn] 풀에 없으면 새로 생성
    // --------------------------------------------------------
    if (!SpawnedMonster)
    {
      //  UE_LOG(LogTemp, Warning, TEXT("Monster Spawned New"));
        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        SpawnedMonster = GetWorld()->SpawnActor<ADEMonsterBase>(
            ClassToSpawn,
            Location,
            FRotator::ZeroRotator,
            Params
        );

        if (!SpawnedMonster) return nullptr;

        // 신규 생성된 녀석은 사망 시 델리게이트 연결 필요
        SpawnedMonster->OnMonsterDeath.AddUObject(this, &ADEMonsterSpawnManager::OnMonsterDied);
    }

    // --------------------------------------------------------
    // 5. [Init] 공통 초기화 (재사용/신규 모두 적용)
    // --------------------------------------------------------
    // 위치 이동 (재사용된 놈은 엉뚱한 곳에 있을 테니까)
    SpawnedMonster->SetActorLocation(Location);
    SpawnedMonster->SetActorRotation(FRotator::ZeroRotator);

    // 몬스터 내부 상태 리셋 (HP, 메쉬 등) -> 아까 만든 InitializeMonster 함수 활용
    //SpawnedMonster->InitializeMonster(DataToApply);
    SpawnedMonster->ResetMonster(DataToApply);


    // 활성 목록에 등록
    ActiveMonsters.Add(SpawnedMonster);

    return SpawnedMonster;
}

void ADEMonsterSpawnManager::ReturnMonsterToPool(ADEMonsterBase* Monster)
{
    if (!Monster) return;

    Monster->ResetForPool();

    Monster->SetActorLocation(FVector::ZeroVector);
 //   UE_LOG(LogTemp, Warning, TEXT("%s Returned to Pool"),*Monster->GetName());
}




FVector ADEMonsterSpawnManager::GetRandomSpawnLocation()
{
    FVector PlayerLocation = Player->GetActorLocation();
    float SpawnX = PlayerLocation.X + FMath::RandRange(-1600.f, 1600.f);
    float SpawnY = PlayerLocation.Y + FMath::RandRange(-1600.f, 1600.f);

    // 화면 안쪽 위치를 제외하고 밖에서만 나오게
    if (FMath::Abs(SpawnX - PlayerLocation.X) < 800.f)
        SpawnX += (SpawnX > PlayerLocation.X ? 800.f : -800.f);
    if (FMath::Abs(SpawnY - PlayerLocation.Y) < 800.f)
        SpawnY += (SpawnY > PlayerLocation.Y ? 800.f : -800.f);

    float SpawnZ = 0.f;
    return FVector(SpawnX, SpawnY, SpawnZ);
}


void ADEMonsterSpawnManager::OnMonsterDied(ADEMonsterBase* Monster)
{
    if (!Monster) return;

    if (UDEPickupManager* PickupMgr = GetWorld()->GetSubsystem<UDEPickupManager>())
    {
        FVector DeathLocation = Monster->GetActorLocation();
        float PlayerLuck = 0.0f; // 필요시 플레이어 스탯에서 캐싱

        // 몬스터의 만능 루트 테이블 굴리기!
        for (const FDEMonsterDropInfo& DropInfo : Monster->DropTable)
        {
            if (!DropInfo.DropClass) continue;

            float Roll = FMath::FRandRange(0.0f, 100.0f);
            float FinalChance = DropInfo.DropChance + (PlayerLuck * 0.1f);

            if (Roll <= FinalChance)
            {
                // 살짝 흩뿌리기 (여러 개 떨어질 때 겹침 방지)
                FVector OffsetLoc = DeathLocation + FVector(FMath::RandRange(-30.f, 30.f), FMath::RandRange(-30.f, 30.f), 0.0f);

                // ★ 대망의 한 줄! (어떤 클래스든, 어떤 밸류든 테이블 값 그대로 스폰!)
                PickupMgr->SpawnPickup(OffsetLoc, DropInfo.ItemValue, DropInfo.DropClass);
            }
        }
    }



    if (Player)
    {
        Player->AddBloodDrainGauge(Player->GetBloodDrainGainPerKill());
    }
    KillCount++;
    // 1. 활성 목록에서 제거
    ActiveMonsters.RemoveSwap(Monster);

    // 2. 비활성(풀) 목록으로 이동
    InactiveMonsters.Add(Monster);

    //UE_LOG(LogTemp, Warning, TEXT("Monster Died, Instantly Removed // TO FIX"));
    ReturnMonsterToPool(Monster);

}

const TArray<ADEMonsterBase*>& ADEMonsterSpawnManager::GetActiveMonsters() const
{
    return ActiveMonsters;
}
