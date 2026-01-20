// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SelectTargetsInRadius.h"
#include "Engine/OverlapResult.h"
#include "DEMonsterBase.h"
#include "DESkillContext.h"

void UDEBehavior_SelectTargetsInRadius::Execute(FDESkillContext& Context)
{
    if (!Context.Instigator) return;
    UWorld* World = Context.Instigator->GetWorld();

    // 1. 중심점 리스트(CenterPoints) 수집
    TArray<FVector> CenterPoints;

    switch (OriginType)
    {
    case ESearchOrigin::Instigator:
        // 나 하나
        CenterPoints.Add(Context.Instigator->GetActorLocation());
        break;

    case ESearchOrigin::Targets:
        // 현재 타겟들 전부 (예: 5명이면 5개 위치)
        for (AActor* Target : Context.Targets)
        {
            if (Target) CenterPoints.Add(Target->GetActorLocation());
        }
        break;

    case ESearchOrigin::CustomLocations:
        // 저장해둔 좌표들 전부 (예: 성수 떨어진 5곳)
        CenterPoints = Context.CustomLocations;
        break;
    }

    if (CenterPoints.Num() == 0) return;


    // 1. 결과 담을 그릇 준비
    TArray<AActor*> RawResults; // 중복 허용 그릇
    TSet<AActor*> UniqueResults; // 중복 방지 그릇

    FCollisionQueryParams Params;
    if (bIgnoreInstigator) Params.AddIgnoredActor(Context.Instigator);
    float FinalRadius = Context.GetValue(TEXT("Radius"), Radius);

    // 2. 루프 돌면서 검색
    for (const FVector& Center : CenterPoints)
    {
        TArray<FOverlapResult> Overlaps;
        bool bHit = World->OverlapMultiByChannel(
            Overlaps, Center, FQuat::Identity,
            ECollisionChannel::ECC_GameTraceChannel2,
            FCollisionShape::MakeSphere(FinalRadius), Params
        );

        if (bHit)
        {
            for (const FOverlapResult& Res : Overlaps)
            {
                AActor* HitActor = Res.GetActor();
                if (HitActor && HitActor->IsA(ADEMonsterBase::StaticClass()))
                {
                    if (bAllowOverlap)
                    {
                        // ★ 옵션 켜짐: 그냥 무식하게 계속 담음 (A, A, A...)
                        RawResults.Add(HitActor);
                    }
                    else
                    {
                        // ★ 옵션 꺼짐: Set에 담아서 중복 거름 (A)
                        UniqueResults.Add(HitActor);
                    }
                }
            }
        }
    }

    // 3. 결과 갱신
    Context.Targets.Empty();

    if (bAllowOverlap)
    {
        // 중복 허용된 리스트 그대로 복사
        Context.Targets = RawResults;
    }
    else
    {
        // 중복 제거된 Set을 배열로 변환
        Context.Targets = UniqueResults.Array();
    }

    //중복 없는 old 
    //// 2. 다중 검색 수행 (결과 합치기)
    //TSet<AActor*> UniqueResults; // 중복 제거를 위해 Set 사용 (A, B 둘 다한테 맞은 놈은 1번만)

    //FCollisionQueryParams Params;
    //if (bIgnoreInstigator) Params.AddIgnoredActor(Context.Instigator);
    //float FinalRadius = Context.GetValue(TEXT("Radius"), Radius);

    //for (const FVector& Center : CenterPoints)
    //{
    //    TArray<FOverlapResult> Overlaps;
    //    bool bHit = World->OverlapMultiByChannel(
    //        Overlaps,
    //        Center,
    //        FQuat::Identity,
    //        ECollisionChannel::ECC_GameTraceChannel2,
    //        FCollisionShape::MakeSphere(FinalRadius),
    //        Params
    //    );

    //    if (bHit)
    //    {
    //        for (const FOverlapResult& Res : Overlaps)
    //        {
    //            if (AActor* HitActor = Res.GetActor())
    //            {
    //                if (HitActor->IsA(ADEMonsterBase::StaticClass()))
    //                {
    //                    UniqueResults.Add(HitActor); // Set이라서 자동 중복 제거됨
    //                }
    //            }
    //        }
    //    }
    //}

    //// 3. 결과 갱신
    //Context.Targets = UniqueResults.Array(); // Set을 다시 Array로 변환해서 저장


}
