// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SpawnOrbit.h"
#include "DEAOE_OrbitBase.h" 
#include "DEAutoSkillBase.h"
#include "DEPoolSubsystem.h"
#include "DESkillContext.h"
#include "DESkillActorBase.h"

void UDEBehavior_SpawnOrbit::Execute(FDESkillContext& Context)
{
    // --------------------------------------------------
   // 0. 유효성 체크
   // --------------------------------------------------
    if (!OrbitAOEClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnOrbit: OrbitAOEClass is null"));
        return;
    }

    // 이전 발동의 궤도가 아직 살아 있으면 (수명이 없거나 쿨다운보다 길 때) 새로 쌓지 않고 재배치한다
    ArrangeOrbits(Context, true);
}

void UDEBehavior_SpawnOrbit::OnContextRefreshed(const FDESkillContext& Context)
{
    // Permanent 궤도 스킬은 재발동 없이 여기로만 레벨업이 들어온다.
    // 아직 발동 전이면 Execute가 새 Context로 스폰하므로 여기서는 스폰하지 않는다
    if (!OrbitAOEClass) return;
    ArrangeOrbits(Context, false);
}

void UDEBehavior_SpawnOrbit::ArrangeOrbits(const FDESkillContext& Context, bool bSpawnIfNone)
{
    if (!Context.Instigator) return;

    UWorld* World = Context.Instigator->GetWorld();
    UDEPoolSubsystem* Pool = World ? World->GetSubsystem<UDEPoolSubsystem>() : nullptr;
    if (!Pool) return;

    TArray<ADEAOE_OrbitBase*> LiveOrbits;
    if (Context.SourceSkill)
    {
        for (const TWeakObjectPtr<ADESkillActorBase>& Actor : Context.SourceSkill->GetLiveSkillActors())
        {
            if (Actor.IsValid() && Actor->IsA(OrbitAOEClass))
            {
                LiveOrbits.Add(Cast<ADEAOE_OrbitBase>(Actor.Get()));
            }
        }
    }

    if (LiveOrbits.Num() == 0 && !bSpawnIfNone) return;

    const int32 TotalAmount = FMath::Max(1, Context.Amount);
    const float AngleStep = 360.0f / static_cast<float>(TotalAmount);

    // 살아 있는 궤도가 있으면 첫 궤도의 현재 각도를 기준으로 균등 재배치해서 궤도가 튀지 않게 한다
    const float BaseAngle = LiveOrbits.Num() > 0 ? LiveOrbits[0]->GetCurrentAngle() : 0.0f;

    for (int32 i = 0; i < TotalAmount; ++i)
    {
        const float Angle = BaseAngle + i * AngleStep;
        if (LiveOrbits.IsValidIndex(i))
        {
            LiveOrbits[i]->InitializeFromContext(Context);
            LiveOrbits[i]->InitOrbit(Angle);
        }
        else
        {
            SpawnOneOrbit(Context, *Pool, Angle);
        }
    }

    // 개수가 줄었으면 남는 궤도는 반환
    for (int32 i = TotalAmount; i < LiveOrbits.Num(); ++i)
    {
        LiveOrbits[i]->ReturnToPool();
    }
}

void UDEBehavior_SpawnOrbit::SpawnOneOrbit(const FDESkillContext& Context, UDEPoolSubsystem& Pool, float StartAngle) const
{
    AActor* Instigator = Context.Instigator;

    // 원점(플레이어)에서 스폰하면 스폰하자마자 플레이어와 겹친 몬스터를 때리므로 궤도 위에서 스폰한다
    const FVector Offset = FRotator(0.f, StartAngle, 0.f).RotateVector(FVector(ADEAOE_OrbitBase::GetOrbitRadius(Context), 0.f, 0.f));
    const FVector SpawnLocation = Instigator->GetActorLocation() + Offset;

    // OrbitAOEClass가 ADEAOE_OrbitBase 계열이라 캐스트가 실패할 일은 없다
    ADEAOE_OrbitBase* OrbitActor = Cast<ADEAOE_OrbitBase>(Pool.GetPooledActor(OrbitAOEClass, SpawnLocation, FRotator::ZeroRotator, false));
    if (!OrbitActor) return;

    OrbitActor->SetOwner(Instigator);
    OrbitActor->InitOrbit(StartAngle);
    OrbitActor->InitializeFromContext(Context);

    // 수명이 없는 궤도는 CancelSkill이 거두도록 스킬에 등록
    if (Context.SourceSkill)
    {
        Context.SourceSkill->RegisterSkillActor(OrbitActor);
    }
}
