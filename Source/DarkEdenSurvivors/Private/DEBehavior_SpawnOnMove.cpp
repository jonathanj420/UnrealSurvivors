// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SpawnOnMove.h"

#include "DEAutoSkillBase.h"
#include "DESkillActorBase.h" // 최상위 스킬 액터 베이스
#include "DEPoolSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UDEBehavior_SpawnOnMove::Execute(FDESkillContext& Context)
{
    if (!Context.Instigator || !Context.SourceSkill) return;
    // 1. 안전한 포인터 캐싱
    CachedInstigator = Context.Instigator;
    CachedSourceSkill = Context.SourceSkill;

    // 2. 최초 기준 위치 기록
    LastSpawnLocation = CachedInstigator->GetActorLocation();

    UWorld* World = CachedInstigator->GetWorld();
    if (World)
    {
        World->GetTimerManager().ClearTimer(DistanceCheckTimerHandle);

        // 3. 이동 검사 타이머 가동
        World->GetTimerManager().SetTimer(
            DistanceCheckTimerHandle,
            this,
            &UDEBehavior_SpawnOnMove::CheckAndSpawn,
            CheckInterval,
            true
        );
    }
}

void UDEBehavior_SpawnOnMove::CheckAndSpawn()
{
    //UE_LOG(LogTemp, Warning, TEXT("Green Poison Try Check and Spawn"));
    // 시전자나 스킬 본체가 사라졌다면 스스로 종료
    if (!CachedInstigator.IsValid() || !CachedSourceSkill.IsValid())
    {
        EndBehavior();
        return;
    }
    //UE_LOG(LogTemp, Warning, TEXT("Green Poison Valid Checked"));
    FVector CurrentLocation = CachedInstigator->GetActorLocation();

    // 1. 거리 검사 (최적화를 위해 DistSquared 사용)
    float DistSq = FVector::DistSquared(CurrentLocation, LastSpawnLocation);
    float ThresholdSq = SpawnDistanceThreshold * SpawnDistanceThreshold;

    if (DistSq >= ThresholdSq)
    {
        // 2. 위치 갱신
        LastSpawnLocation = CurrentLocation;

        UWorld* World = CachedInstigator->GetWorld();
        if (!World || !SpawnActorClass) return;
        //UE_LOG(LogTemp, Warning, TEXT("Green Poison Checked Spawn Actor Class"));
        UDEPoolSubsystem* Pool = World->GetSubsystem<UDEPoolSubsystem>();
        if (!Pool) return;

        // =========================================================
        // 3. 갱신된 최신 스탯을 가져와 컨텍스트 새로 구성
        // =========================================================
        FDESkillContext FreshContext;
        CachedSourceSkill->BuildContext(FreshContext);

        FreshContext.CustomLocations.Empty();
        FreshContext.CustomLocations.Add(CurrentLocation);

        // 4. 오브젝트 풀에서 만능 액터 꺼내기
        AActor* RawActor = Pool->GetPooledActor(SpawnActorClass, CurrentLocation, FRotator::ZeroRotator, false);

        // ★ 다형성의 핵심: 뭐가 나오든 ADESkillActorBase로 받아서 처리!
        if (ADESkillActorBase* SkillActor = Cast<ADESkillActorBase>(RawActor))
        {
            SkillActor->SetOwner(CachedInstigator.Get());

            // 최신 스탯 주입 (오버라이드된 각자의 함수가 실행됨)
            SkillActor->InitializeFromContext(FreshContext);
           // UE_LOG(LogTemp, Error, TEXT("Spawned By Distance"));
            // 5. CancelSkill이 거두도록 스킬에 등록
            CachedSourceSkill->RegisterSkillActor(SkillActor);
        }
    }
}

void UDEBehavior_SpawnOnMove::OnContextRefreshed(const FDESkillContext& Context)
{
}

void UDEBehavior_SpawnOnMove::EndBehavior()
{
    // 스킬 진화 및 해제 시 깔끔하게 타이머 해제
    if (CachedInstigator.IsValid())
    {
        UWorld* World = CachedInstigator->GetWorld();
        if (World)
        {
            World->GetTimerManager().ClearTimer(DistanceCheckTimerHandle);
        }
    }
}