// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SelectTargetsBySweep.h"
#include "DrawDebugHelpers.h"
#include "DESkillContext.h"

void UDEBehavior_SelectTargetsBySweep::Execute(FDESkillContext& Context)
{
    if (!Context.Instigator) return;
    UWorld* World = Context.Instigator->GetWorld();

    FVector StartLoc = Context.Instigator->GetActorLocation();

    // 1. 기본 방향은 플레이어의 정면 (타겟이 없을 때를 대비한 안전장치)
    FVector Direction = Context.Instigator->GetActorForwardVector();

    // 2. 이전 Behavior가 Context.Targets에 타겟을 담아뒀다면? 그쪽을 향한다!
    if (Context.Targets.Num() > 0 && Context.Targets[0] != nullptr)
    {
        Direction = (Context.Targets[0]->GetActorLocation() - StartLoc).GetSafeNormal();
    }

    // 3. 스펙 세팅 (Context에 Range가 없다면, Radius 변수 등을 적절히 활용해 주세요)
    float FinalRadius = Context.Radius;
    float FinalRange = Context.GetValue(TEXT("Range"), 600.0f);

    FVector EndLoc = StartLoc + (Direction * FinalRange);

    TSet<AActor*> UniqueResults;
    FCollisionQueryParams Params;
    //if (bIgnoreInstigator) Params.AddIgnoredActor(Context.Instigator);

    TArray<FHitResult> HitResults;

    // 4. 스윕 쏘기
    bool bHit = World->SweepMultiByProfile(
        HitResults,
        StartLoc,
        EndLoc,
        FQuat::Identity,
        FName(TEXT("PlayerAttack")),
        FCollisionShape::MakeSphere(FinalRadius),
        Params
    );

    // 5. 이전 타겟(1마리) 정보는 지우고, 스윕으로 긁어온 애들로 배열 덮어씌우기
    Context.Targets.Empty();
    if (HitResults.Num() > 0)
    {
        for (const FHitResult& Res : HitResults)
        {
            AActor* HitActor = Res.GetActor();
            if (HitActor)
            {
                UniqueResults.Add(HitActor);
            }
        }
        Context.Targets = UniqueResults.Array();
    }

   

}
