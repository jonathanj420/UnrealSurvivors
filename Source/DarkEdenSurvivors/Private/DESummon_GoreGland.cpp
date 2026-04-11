// Fill out your copyright notice in the Description page of Project Settings.


#include "DESummon_GoreGland.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Engine/OverlapResult.h"
#include "DEGameplayLibrary.h"
#include "NiagaraFunctionLibrary.h"

ADESummon_GoreGland::ADESummon_GoreGland()
{
    PrimaryActorTick.bCanEverTick = true;
    TargetSearchRadius = -1.0f;

}

void ADESummon_GoreGland::ResetState()
{
    Super::ResetState();

    CurrentTarget = nullptr;

    // 1. [타겟 탐색] 0.5초마다 가벼운 탐색만 진행
    GetWorldTimerManager().SetTimer(TargetSearchTimerHandle, this, &ADESummon_GoreGland::FindNearestTarget, 0.5f, true);

    // 2. [주기적 타격] 피 뿜기 무한 반복 타이머
    GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ADESummon_GoreGland::PerformBloodPulse, AttackInterval, true);
}

void ADESummon_GoreGland::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // [이동 로직] 틱마다 부드럽게 타겟을 향해 기어갑니다.
    if (IsValid(CurrentTarget))
    {
        FVector CurrentLoc = GetActorLocation();
        FVector TargetLoc = CurrentTarget->GetActorLocation();

        // Z축 고정: 공중에 뜨지 않고 피 장판이 바닥을 예쁘게 쓸고 다니게 합니다.
        TargetLoc.Z = CurrentLoc.Z;

        FVector Direction = (TargetLoc - CurrentLoc).GetSafeNormal();
        SetActorLocation(CurrentLoc + (Direction * MoveSpeed * DeltaTime));

        // (선택) 진행 방향으로 살짝 회전하게 만들면 꿈틀거리는 느낌이 더 삽니다.
        FRotator NewRot = FMath::RInterpTo(GetActorRotation(), Direction.Rotation(), DeltaTime, 5.0f);
        SetActorRotation(NewRot);
    }
}

void ADESummon_GoreGland::FindNearestTarget()
{
    // 이미 쫓고 있는 타겟이 살아있고 사거리 내에 있다면 갱신 생략 (유지)
    if (IsValid(CurrentTarget))
    {
        float DistSq = FVector::DistSquared(GetActorLocation(), CurrentTarget->GetActorLocation());
        if (DistSq < FMath::Square(TargetSearchRadius))
        {
            return;
        }
    }

    // ★ [핵심] 라이브러리 호출로 길었던 오버랩 코드를 단 1줄로 압축!
    CurrentTarget = UDEGameplayLibrary::GetNearestTarget(this, TargetSearchRadius);

}

void ADESummon_GoreGland::PerformBloodPulse()
{
    UWorld* World = GetWorld();
    if (!World) return;

    FVector Center = GetActorLocation();

    // ★ [이펙트 연동] 방금 깎으신 나이아가라 시스템에 "지금 터져라!" 하고 신호를 보냅니다.
    // (나이아가라 내부에서 'TriggerPulse'라는 int32 변수를 만들어두고 User Parameter로 받게 세팅하시면 됩니다)
    

    //// --- 광역 타격 스캔 ---
    //TArray<FOverlapResult> OverlapResults;
    //FCollisionShape SphereShape = FCollisionShape::MakeSphere(EffectRadius);
    //FCollisionQueryParams QueryParams;
    //QueryParams.AddIgnoredActor(this);
    //if (CachedContext.Instigator) QueryParams.AddIgnoredActor(CachedContext.Instigator);

    //GetWorld()->OverlapMultiByProfile(OverlapResults, Center, FQuat::Identity, TEXT("Monster"), SphereShape, QueryParams);

    //// 이번 펄스에서 이미 맞은 놈은 두 번 맞지 않게 방지
    //TSet<AActor*> DamagedActors;

    //for (const FOverlapResult& Result : OverlapResults)
    //{
    //    AActor* Victim = Result.GetActor();
    //    if (Victim && !DamagedActors.Contains(Victim))
    //    {
    //        DamagedActors.Add(Victim);

    //        // 부모(SummonBase)에 만들어둔 갓벽한 데미지 함수 호출!
    //        TryDealDamage(Victim);
    //    }
    //}

    FCollisionShape SphereShape = FCollisionShape::MakeSphere(EffectRadius);
    //DrawDebugSphere(World, Center, EffectRadius, 12, FColor::Green, false, 1.0f, 0, 1.0f);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(CachedContext.Instigator);
    QueryParams.bTraceComplex = false;

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel5);

    TArray<FOverlapResult> OverlapResults;

    /*bool bHit = World->OverlapMultiByProfile(
        OverlapResults,
        Center,
        FQuat::Identity,
        FName(TEXT("PlayerAttack")),
        SphereShape,
        QueryParams);*/

    bool bHitAny = World->OverlapMultiByObjectType(
        OverlapResults,
        Center,
        FQuat::Identity,
        ObjectQueryParams, // ★ 채널 대신 오브젝트 쿼리 파라미터가 들어감
        SphereShape,
        QueryParams
    );

    //Context.Targets.Reset(); // 찌꺼기 비우기

    //THIS MOFOKIN OverlapMultiByProfile NEVER RETURNS FUCKIN TRUE FOR OVERLAPS ONLY TRUE FOR FUCKING BLOCKS
    if (bHitAny)
    {
        // 중복 타격 방지용 (한 몬스터에 콜라이더가 여러 개일 수 있음)
        //TSet<AActor*> HitActors;
        //UE_LOG(LogTemp, Warning, TEXT("Target Hit"));
        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* TargetActor = Result.GetActor();
            if (TargetActor)
            {
                TryDealDamage(TargetActor);

            }
        }


    }


    if (BloodPoolFX)
    {
        UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            World,
            BloodPoolFX,
            Center,
            GetActorRotation()
        );

        if (SpawnedEffect)
        {
            UE_LOG(LogTemp, Warning, TEXT("tje fuck? : %f "), EffectRadius);
            // 물리 타격 판정과 완전히 동일한 'EffectRadius'를 나이아가라로 쏴줌!
            SpawnedEffect->SetFloatParameter(FName("SkillRadius"), EffectRadius);
        }
    }
}

void ADESummon_GoreGland::ReturnToPool()
{
    // 풀로 반환될 때 루프 타이머 깔끔하게 끄기
    GetWorldTimerManager().ClearTimer(TargetSearchTimerHandle);
    GetWorldTimerManager().ClearTimer(AttackTimerHandle);
    CurrentTarget = nullptr;

    Super::ReturnToPool();
}
