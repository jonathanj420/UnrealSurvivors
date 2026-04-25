// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SpawnOnMove.h"

#include "DEAutoSkillBase.h"
#include "DESkillActorBase.h" // 최상위 스킬 액터 베이스
#include "DEPoolSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UDEBehavior_SpawnOnMove::Execute(FDESkillContext& Context)
{
    UE_LOG(LogTemp, Warning, TEXT("Try Green Poison Execution"));
    if (!Context.Instigator || !Context.SourceSkill) return;
    UE_LOG(LogTemp, Warning, TEXT("Green Poison Checked Instigator"));
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
            // 5. 실시간 리프레시를 위해 명부에 등록
            FString UniqueStr = FGuid::NewGuid().ToString();
            FName Key = FName(*FString::Printf(TEXT("%s_%s"), *SpawnActorClass->GetName(), *UniqueStr));
            CachedSourceSkill->OwnedSkillActorMap.Add(Key, SkillActor);
        }
    }
}

void UDEBehavior_SpawnOnMove::OnContextRefreshed(const FDESkillContext& Context)
{
    //if (!Context.SourceSkill) return;

    //TArray<FName> KeysToRemove;

    //// 1. 명부에 적힌 모든 액터 순회 (장판이든 소환수든 가리지 않음)
    //for (auto& Pair : Context.SourceSkill->OwnedSkillActorMap)
    //{
    //    if (Pair.Value.IsValid())
    //    {
    //        if (auto* SkillActor = Pair.Value.Get())
    //        {
    //            // 실시간 스탯 덮어씌우기
    //            SkillActor->InitializeFromContext(Context);
    //        }
    //    }
    //    else
    //    {
    //        // 수명이 다해 파괴되었거나 Pool로 돌아간 액터 수집
    //        KeysToRemove.Add(Pair.Key);
    //    }
    //}

    //// 2. 찌꺼기 메모리 청소
    //for (const FName& DeadKey : KeysToRemove)
    //{
    //    Context.SourceSkill->OwnedSkillActorMap.Remove(DeadKey);
    //}
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