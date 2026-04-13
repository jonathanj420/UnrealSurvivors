// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SpawnSummon.h"
#include "DEPoolSubsystem.h"
#include "DESimpleSummonBase.h"
#include "Kismet/GameplayStatics.h"
#include "DEAutoSkillBase.h"

void UDEBehavior_SpawnSummon::Execute(FDESkillContext& Context)
{
    if (!Context.Instigator || !SummonClass) return;

    UWorld* World = Context.Instigator->GetWorld();
    if (!World) return;

    UDEPoolSubsystem* Pool = World->GetSubsystem<UDEPoolSubsystem>();
    if (!Pool) return;

    // 1. [스폰 위치 계산]
    FVector OwnerLoc = Context.Instigator->GetActorLocation();
    FRotator OwnerRot = Context.Instigator->GetActorRotation();

    // 만약 Targeting 비헤이비어를 통해 Target이 잡혀있다면 적 위치에 스폰!
    FVector SpawnLoc;
    if (Context.Targets.Num() > 0 && Context.Targets[0])
    {
        SpawnLoc = Context.Targets[0]->GetActorLocation();
    }
    else
    {
        // 타겟이 없다면 내 앞쪽으로 살짝 띄워서 스폰
        FVector2D RandomOffset2D = FMath::RandPointInCircle(SpawnDistanceOffset);

        //FVector ForwardDir = Context.Instigator->GetActorForwardVector();
        SpawnLoc = OwnerLoc + FVector(RandomOffset2D.X, RandomOffset2D.Y, 0.0f);

    }

    // Z축은 바닥으로 고정 (필요 시)
    SpawnLoc.Z = OwnerLoc.Z;

    // 2. [오브젝트 풀에서 꺼내기]
    int32 TargetCount = Context.Amount; // 기본적으로는 최종 스탯 보따리의 값을 쓴다.

    if (bUseBaseAmountOnly && Context.SourceSkill)
    {
        // 원본 스킬(SourceSkill) 객체나 데이터 에셋에 직접 접근하여 순수 값을 가져온다.
        // (GetBaseAmount는 기존 스킬 클래스에 구현해두신 데이터 테이블 조회 함수라고 가정)
        TargetCount = Context.SourceSkill->GetBaseAmount();
    }

    int32 Count = FMath::Max(1, TargetCount);

    for (int32 i = 0; i < Count; i++)
    {
        // 여러 마리일 경우 위치를 살짝씩 흩뿌려줌
        FVector FinalLoc = SpawnLoc + FVector(FMath::RandRange(-50.f, 50.f), FMath::RandRange(-50.f, 50.f), 0.f);

        AActor* PooledActor = Pool->GetPooledActor(SummonClass, FinalLoc, FRotator::ZeroRotator, false);

        if (auto* Summon = Cast<ADESimpleSummonBase>(PooledActor))
        {
            // 3. [초기화] (여기서 Context.Duration 등의 정보가 넘어감)
            Summon->InitializeFromContext(Context);
        }
    }

    // 4. [사운드]
    if (SpawnSound)
    {
        UGameplayStatics::PlaySoundAtLocation(World, SpawnSound, SpawnLoc);
    }
}

