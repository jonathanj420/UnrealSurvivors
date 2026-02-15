// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SpawnAOE.h"
#include "DEAutoSkillBase.h"          // ActiveSkill 접근용
#include "DESkillContext.h"        // 컨텍스트 정의
#include "DESimpleAOEBase.h"      // 장판 액터 베이스
#include "DEPoolSubsystem.h"       // 풀링 서브시스템
#include "DECharacterBase.h"
#include "Kismet/GameplayStatics.h"
void UDEBehavior_SpawnAOE::Execute(FDESkillContext& Context)
{

    // --------------------------------------------------
     // 0. 유효성 체크
     // --------------------------------------------------
    if (Context.AOERequests.Num() == 0)
    {
        return;
    }

    AActor* Instigator = Context.Instigator;
    if (!Instigator)
    {
        return;
    }

    UWorld* World = Instigator->GetWorld();
    if (!World)
    {
        return;
    }

    UGameInstance* GI = World->GetGameInstance();
    if (!GI)
    {
        return;
    }

    UDEPoolSubsystem* Pool = World->GetSubsystem<UDEPoolSubsystem>();
    if (!Pool)
    {
        return;
    }

    // Player 기준 Persistent 관리
    ADECharacterBase* Player = Cast<ADECharacterBase>(Instigator);
    UE_LOG(LogTemp, Warning, TEXT("Try Spawn AOE"));
    // --------------------------------------------------
    // 1. AOE 요청 처리
    // --------------------------------------------------
    for (const FAOERequest& Request : Context.AOERequests)
    {
        if (!Request.AOEClass)
        {
            continue;
        }

        ADESimpleAOEBase* AOE = nullptr;
        const bool bHasKey = !Request.AOEKey.IsNone();

        // ----------------------------------------------
        // 1-A. Persistent AOE 재사용 시도
        // ----------------------------------------------
        if (bHasKey && Player)
        {
            if (TWeakObjectPtr<ADESimpleAOEBase>* Found =
                Player->ActiveAOEMap.Find(Request.AOEKey))
            {
                if (Found->IsValid())
                {
                    AOE = Found->Get();
                    UE_LOG(LogTemp, Warning, TEXT("Persistent AOE"));
                }
            }
        }

        // ----------------------------------------------
        // 1-B. 없으면 Pool에서 새로 확보
        // ----------------------------------------------
        bool bIsNewSpawn = false;
        if (!AOE)
        {
            AActor* RawActor = Pool->GetPooledActor(
                Request.AOEClass,
                Request.SpawnLocation,
                FRotator::ZeroRotator,
                /*bAutoActivate=*/false
            );
            UE_LOG(LogTemp, Warning, TEXT("AOE : %s Get Pooled Actor"), *RawActor->GetName());
            AOE = Cast<ADESimpleAOEBase>(RawActor);
            if (!AOE)
            {
                continue;
            }
            UE_LOG(LogTemp, Warning, TEXT("AOE : Casted Successfully"), *AOE->GetName());
            AOE->SetOwner(Instigator);
            bIsNewSpawn = true;
            // Persistent 등록
            if (bHasKey && Player)
            {
                Player->ActiveAOEMap.Add(Request.AOEKey, AOE);
            }
        }
        else
        {
            // 재사용 시 위치만 갱신
            AOE->SetActorLocation(Request.SpawnLocation);
            if (AOE->GetOwner() != Instigator)
            {
                AOE->SetOwner(Instigator);

            }
        }

        // ----------------------------------------------
        // 1-C. 핵심: Context 재적용
        // ----------------------------------------------
        AOE->ApplyContext(Context);
        UE_LOG(LogTemp, Error, TEXT("AOE Context Applied, Damage : %f, Radius %f, Tick : %f"), Context.Damage, Context.Radius, Context.GetValue(TEXT("HitInterval"), 0.5f));
        AOE->ActivateAOE(bIsNewSpawn);
        // ----------------------------------------------
        // 1-D. 결과 기록
        // ----------------------------------------------
        Context.SpawnedAOEs.Add(AOE);
    }

    //// 1. 필수 데이터 검증
    //if (!Context.Instigator || !Context.ActiveSkill)
    //{
    //	// 로그: 주인이 없거나 스킬 인스턴스가 없음
    //	return;
    //}

    //// 2. 이미 소환된 오라가 있는지 확인 (스킬 인스턴스에 저장된 변수)
    //// Cast를 통해 우리가 관리하는 AOE 클래스인지 확인
    //ADESimpleAOEBase* ExistingAura = Cast<ADESimpleAOEBase>(Context.ActiveSkill->SpawnedAura);

    //// -------------------------------------------------------
    //// CASE A: 이미 오라가 존재함 (리필 & 갱신)
    //// -------------------------------------------------------
    //if (ExistingAura && IsValid(ExistingAura))
    //{
    //	// A-1. 스탯 갱신 (레벨업으로 데미지나 범위가 바뀌었을 수 있음)
    //	// 여기서 데미지, 범위(Scale), 타격간격 등이 최신 상태로 바뀜
    //	ExistingAura->InitializeFromContext(Context);

    //	// A-2. [핵심] 수명 연장 (Refill)
    //	// Duration이 0보다 클 때만 수명을 다시 세팅해줌 (0이면 무한이니까 건드리지 않음)
    //	if (Context.Duration > 0.f)
    //	{
    //		ExistingAura->SetLifeSpan(Context.Duration);
    //	}

    //	// (디버깅용 로그)
    //	// UE_LOG(LogTemp, Log, TEXT("AOE Refreshed: %s"), *ExistingAura->GetName());
    //}
    //// -------------------------------------------------------
    //// CASE B: 오라가 없음 (신규 소환)
    //// -------------------------------------------------------
    //else
    //{
    //	// B-1. 풀 서브시스템 가져오기
    //	UWorld* World = Context.Instigator->GetWorld();
    //	if (!World) return;

    //	auto* PoolSubsystem = World->GetGameInstance()->GetSubsystem<UDEPoolSubsystem>();
    //	if (!PoolSubsystem) return;

    //	// B-2. 소환할 클래스 확인 (Context의 ProjectileClass 사용)
    //	UClass* ClassToSpawn = Context.ProjectileClass;
    //	if (!ClassToSpawn)
    //	{
    //		// 만약 Context에 없다면, 기본값이나 에러 처리
    //		UE_LOG(LogTemp, Warning, TEXT("SpawnAOE: ProjectileClass is null in Context!"));
    //		return;
    //	}

    //	// B-3. 풀에서 스폰 (위치는 주인 위치)
    //	ADESimpleAOEBase* NewAura = PoolSubsystem->SpawnFromPool<ADESimpleAOEBase>(
    //		ClassToSpawn,
    //		Context.Instigator->GetActorLocation(),
    //		FRotator::ZeroRotator
    //	);

    //	if (NewAura)
    //	{
    //		// B-4. 플레이어에게 부착 (따라다니게 설정)
    //		// SnapToTargetNotIncludingScale: 위치/회전은 딱 붙이고, 스케일은 유지
    //		NewAura->AttachToComponent(
    //			Context.Instigator->GetRootComponent(),
    //			FAttachmentTransformRules::SnapToTargetNotIncludingScale
    //		);

    //		// B-5. 초기화 (스탯 적용)
    //		NewAura->InitializeFromContext(Context);

    //		// B-6. [중요] 스킬 인스턴스에 "이 녀석이 내 오라다"라고 등록
    //		Context.ActiveSkill->SpawnedAura = NewAura;
    //	}
    //}
}
