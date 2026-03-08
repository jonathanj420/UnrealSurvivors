// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_CullByCone.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "CollisionShape.h"
#include "CollisionQueryParams.h"

// --- 디버그 드로잉 ---
#include "DrawDebugHelpers.h"
#include "DESkillContext.h"

void UDEBehavior_CullByCone::Execute(FDESkillContext& Context)
{
    bool bShowDebug = true;
    if (!Context.Instigator) return;

    UWorld* World = Context.Instigator->GetWorld();
    if (!World) return;

    FVector MyLoc = Context.Instigator->GetActorLocation();
    FVector ForwardDir = Context.Instigator->GetActorForwardVector();

    if (Context.Targets.Num() > 0 && Context.Targets[0])
    {
        // (타겟 위치 - 내 위치)를 하면 타겟을 향한 '방향 벡터'가 나옵니다.
        ForwardDir = (Context.Targets[0]->GetActorLocation() - Context.Instigator->GetActorLocation()).GetSafeNormal();
    }

    // 1. Context에 동적 스탯이 있으면 쓰고, 없으면 기본값(this) 사용
    float FinalRadius = (Context.Radius > 0.f) ? Context.Radius : this->Radius;
    float FinalAngle = Context.GetValue(TEXT("Angle"), 60.0f);


    // ==========================================
    // 2. 부채꼴 디버그 그리기 (피자 조각)
    // ==========================================
    if (bShowDebug)
    {
        float HalfAngle = FinalAngle / 2.0f;
        FVector LeftDir = ForwardDir.RotateAngleAxis(-HalfAngle, FVector::UpVector);
        FVector RightDir = ForwardDir.RotateAngleAxis(HalfAngle, FVector::UpVector);

        FVector LeftEnd = MyLoc + (LeftDir * FinalRadius);
        FVector RightEnd = MyLoc + (RightDir * FinalRadius);

        // 양옆 테두리 (빨간색, 2초 유지)
        DrawDebugLine(World, MyLoc, LeftEnd, FColor::Red, false, 0.5f, 0, 2.0f);
        DrawDebugLine(World, MyLoc, RightEnd, FColor::Red, false, 0.5f, 0, 2.0f);

        // 둥근 호(Arc) 그리기
        int32 Segments = FMath::Max(4, FMath::CeilToInt(FinalAngle / 10.0f));
        float AngleStep = FinalAngle / Segments;
        FVector PrevPoint = LeftEnd;

        for (int32 i = 1; i <= Segments; i++)
        {
            float CurrentAngle = -HalfAngle + (AngleStep * i);
            FVector CurrentDir = ForwardDir.RotateAngleAxis(CurrentAngle, FVector::UpVector);
            FVector CurrentPoint = MyLoc + (CurrentDir * FinalRadius);

            DrawDebugLine(World, PrevPoint, CurrentPoint, FColor::Red, false, 0.5f, 0, 2.0f);
            PrevPoint = CurrentPoint;
        }
    }

    // ==========================================
    //  3. UWorld 다이렉트 충돌 검사 (최적화)
    // ==========================================
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(FinalRadius);
    DrawDebugSphere(World, MyLoc, FinalRadius, 12, FColor::Green, false, 1.0f, 0, 1.0f);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Context.Instigator);
    QueryParams.bTraceComplex = false;

    TArray<FOverlapResult> OverlapResults;

    bool bHit = World->OverlapMultiByProfile(
        OverlapResults,
        MyLoc,
        FQuat::Identity,
        FName(TEXT("PlayerAttack")),
        SphereShape,
        QueryParams);

    Context.Targets.Reset(); // 찌꺼기 비우기

    //THIS MOFOKIN OverlapMultiByProfile NEVER RETURNS FUCKIN TRUE FOR OVERLAPS BUT FALSE FOR FUCKING BLOCKS
    if (OverlapResults.Num() > 0)
    {
        float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(FinalAngle / 2.0f));

        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* TargetActor = Result.GetActor();
            if (!TargetActor) continue;
            // 타겟을 향하는 방향 벡터
            FVector DirToTarget = (TargetActor->GetActorLocation() - MyLoc).GetSafeNormal();

            // 내적 계산
            float DotResult = FVector::DotProduct(ForwardDir, DirToTarget);

            // 부채꼴 각도 안에 들어왔다면? 합격!
            if (DotResult >= CosHalfAngle)
            {
                Context.Targets.Add(TargetActor);

                // 디버그 켜져 있으면 맞은 놈 몸에 초록색 구체 띄워주기!
                if (bShowDebug)
                {
                    DrawDebugSphere(World, TargetActor->GetActorLocation(), 50.0f, 12, FColor::Green, false, 2.0f, 0, 2.0f);
                }
            }
        }
    }

}
