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
    //bool bDrawDebug = true;

    //if (bDrawDebug)
    //{
    //    // 1. 캡슐의 중심점 계산 (시작점과 끝점의 정확히 한가운데)
    //    FVector Center = StartLoc + (Direction * (FinalRange * 0.5f));

    //    // 2. 캡슐의 절반 높이 계산 (이동 거리의 절반 + 양 끝에 튀어나온 구체의 반지름)
    //    float HalfHeight = (FinalRange * 0.5f) + FinalRadius;

    //    // 3. 캡슐 회전값 계산 ★ (매우 중요)
    //    // 언리얼의 캡슐은 기본적으로 Z축(위쪽)으로 서 있습니다.
    //    // 우리가 쏘는 레이저 방향(Direction)이 캡슐의 기둥(Z축)이 되도록 눕혀줍니다.
    //    FQuat CapsuleRot = FRotationMatrix::MakeFromZ(Direction).ToQuat();

    //    // 4. 진짜 그리기! (적중했으면 초록색, 허공을 갈랐으면 빨간색)
    //    DrawDebugCapsule(
    //        World,
    //        Center,
    //        HalfHeight,
    //        FinalRadius,
    //        CapsuleRot,
    //        bHit ? FColor::Green : FColor::Red, // 색상
    //        false, // 영구 유지 여부
    //        2.0f,  // 화면에 남아있을 시간 (2초)
    //        0,     // Depth Priority
    //        2.0f   // 선 두께
    //    );

    //    // (선택) 시작점과 끝점에 작은 구를 그려주면 확실히 파악하기 더 좋습니다.
    //    DrawDebugSphere(World, StartLoc, 10.0f, 8, FColor::Blue, false, 2.0f);
    //    DrawDebugSphere(World, EndLoc, 10.0f, 8, FColor::Yellow, false, 2.0f);
    //}

   

}
