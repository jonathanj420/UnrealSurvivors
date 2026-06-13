// Fill out your copyright notice in the Description page of Project Settings.


#include "DEMonsterUpdateComponent.h"
#include "DEMonsterBase.h"
#include "DECharacterBase.h"

UDEMonsterUpdateComponent::UDEMonsterUpdateComponent()
{
    // 이 컴포넌트 자체의 Tick은 끔 - SpawnManager Tick에서 직접 호출할 거라서
    PrimaryComponentTick.bCanEverTick = false;
}

void UDEMonsterUpdateComponent::UpdateMonsters(float DeltaTime, TArray<ADEMonsterBase*>& ActiveMonsters)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UDEMonsterUpdateComponent::UpdateMonsters);
    if (!Player || ActiveMonsters.Num() == 0) return;

    // [완벽한 최적화] 프레임 시작할 때, 모든 몬스터의 위치를 각자의 주머니에 딱 1번만 캐싱!
    // 이제 이 프레임이 끝날 때까지 GetActorLocation()은 절대 부르지 않습니다.
    const int32 MonsterCount = ActiveMonsters.Num();
    for (int32 i = 0; i < MonsterCount; ++i)
    {
        if (ActiveMonsters[i])
        {
            ActiveMonsters[i]->CachedLocation = ActiveMonsters[i]->GetActorLocation();
        }
    }

    UpdateMovementAndAttack(DeltaTime, ActiveMonsters);
    ResolveOverlaps(ActiveMonsters);
}

// -------------------------------------------------------
// 이동 + 공격
// -------------------------------------------------------
void UDEMonsterUpdateComponent::UpdateMovementAndAttack(float DeltaTime, TArray<ADEMonsterBase*>& ActiveMonsters)
{
    const FVector PlayerLocation = Player->GetActorLocation();
    const double CurrentWorldTime = GetWorld()->GetTimeSeconds();
    const float PlayerRadius = Player->GetCapsuleHalfRadius();

    for (int32 i = ActiveMonsters.Num() - 1; i >= 0; --i)
    {
        ADEMonsterBase* Mob = ActiveMonsters[i];
        if (!Mob) continue;

        Mob->UpdateKnockback(DeltaTime);
        if (!Mob->bCanMove) continue;

        Mob->MoveToPlayer(DeltaTime, PlayerLocation, Mob->CachedLocation);
        Mob->ExecuteAttackLogic(CurrentWorldTime);

        // [수정] 배열에서 꺼내지 말고, 아까 위에서 구워둔 Mob 주머니(CachedLocation)에서 꺼냄!
        ResolvePlayerPush(Mob, PlayerLocation, Mob->CachedLocation, PlayerRadius);
    }

}

// -------------------------------------------------------
// 몬스터 간 겹침 해소 O(n^2) - 추후 공간분할로 개선 여지 있음
// -------------------------------------------------------
void UDEMonsterUpdateComponent::ResolveOverlaps(TArray<ADEMonsterBase*>& ActiveMonsters)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UDEMonsterUpdateComponent::ResolveOverlaps);

    const int32 MonsterCount = ActiveMonsters.Num();

    /// ★ 0. 이전 프레임에 사용된 셀만 비우기 (전체 map 순회 제거!!!)
    for (const FIntPoint& Cell : LastFrameTouchedCells)
    {
        if (TArray<ADEMonsterBase*>* Arr = SpatialGrid.Find(Cell))
        {
            Arr->Reset();
        }
    }
    LastFrameTouchedCells.Reset();



    // 2. 몬스터들을 그리드 셀에 배정
    TSet<FIntPoint> ThisFrameCells;
    for (int32 i = 0; i < MonsterCount; ++i)
    {
        ADEMonsterBase* Mob = ActiveMonsters[i];
        if (!Mob) continue;

        const FVector& Loc = Mob->CachedLocation; // 함수 호출 없이 변수만 쏙 빼먹음!
        FIntPoint CellIndex(FMath::FloorToInt(Loc.X / CellSize), FMath::FloorToInt(Loc.Y / CellSize));
        SpatialGrid.FindOrAdd(CellIndex).Add(Mob);
        ThisFrameCells.Add(CellIndex);  // 중복 자동 방지
    }

    

    // =========================================================
    // 3. 충돌 검사 (몬스터 중심 -> 셀 단위 4방향 하프-네이버 최적화!)
    // =========================================================
    for (const FIntPoint& CellIndex : ThisFrameCells)
    {
        TArray<ADEMonsterBase*>* CellMonstersPtr = SpatialGrid.Find(CellIndex);
        if (!CellMonstersPtr || CellMonstersPtr->Num() == 0) continue;

        TArray<ADEMonsterBase*>& CellMonsters = *CellMonstersPtr;
        const int32 CellMobCount = CellMonsters.Num();

        // 3-1. [같은 셀 내부 검사] 
        // A < B 비교할 필요 없이, 인덱스(i, j) 조합으로 중복 없이 딱 1번씩만 매칭!
        for (int32 i = 0; i < CellMobCount; ++i)
        {
            for (int32 j = i + 1; j < CellMobCount; ++j)
            {
                ResolveMonsterOverlap(CellMonsters[i], CellMonsters[j], CellMonsters[i]->CachedLocation, CellMonsters[j]->CachedLocation);
            }
        }

        // 3-2. [주변 셀 검사] 
        // 9방향 버리고, 미래 방향(우, 우하, 하, 좌하) 4곳만 검사!
        FIntPoint NeighborOffsets[4] = { FIntPoint(1, 0), FIntPoint(1, 1), FIntPoint(0, 1), FIntPoint(-1, 1) };

        for (int32 dir = 0; dir < 4; ++dir)
        {
            if (TArray<ADEMonsterBase*>* NeighborMonsters = SpatialGrid.Find(CellIndex + NeighborOffsets[dir]))
            {
                if (NeighborMonsters->Num() == 0) continue;

                // 내 셀의 몬스터들과, 인접한 4방향 셀의 몬스터들을 교차 검증
                for (ADEMonsterBase* MobA : CellMonsters)
                {
                    for (ADEMonsterBase* MobB : *NeighborMonsters)
                    {
                        // 여기선 소속된 셀 자체가 완전히 다르므로, (MobA < MobB) 검사 없이 냅다 밀어버려도 중복 절대 안 생김!
                        ResolveMonsterOverlap(MobA, MobB, MobA->CachedLocation, MobB->CachedLocation);
                    }
                }
            }
        }
    }

    //// 3. 충돌 검사
    //for (int32 i = 0; i < MonsterCount; ++i)
    //{
    //    ADEMonsterBase* MobA = ActiveMonsters[i];
    //    if (!MobA) continue;

    //    const FVector& LocA = MobA->CachedLocation;
    //    FIntPoint CellA(FMath::FloorToInt(LocA.X / CellSize), FMath::FloorToInt(LocA.Y / CellSize));

    //    for (int32 dx = -1; dx <= 1; ++dx)
    //    {
    //        for (int32 dy = -1; dy <= 1; ++dy)
    //        {
    //            if (TArray<ADEMonsterBase*>* CellMonsters = SpatialGrid.Find(FIntPoint(CellA.X + dx, CellA.Y + dy)))
    //            {
    //                for (ADEMonsterBase* MobB : *CellMonsters)
    //                {
    //                    if (MobA < MobB)
    //                    {
    //                        // MobB 위치도 함수 호출 없이 변수로 바로 전달!
    //                        ResolveMonsterOverlap(MobA, MobB, LocA, MobB->CachedLocation);
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}

    // 4. 이동 결재 (엔진의 헛수고 방지)
    for (ADEMonsterBase* Mob : ActiveMonsters)
    {
        if (!Mob) continue;

        //  [부활] 밀어낼 힘이 있을 때'만' 엔진을 깨운다! (이거 진짜 중요합니다)
        if (!Mob->PendingOverlapPush.IsNearlyZero())
        {
            // 밀어내기 벡터 (주의: 여기에 MoveToPlayer 이동량까지 합쳐져 있다면 Clamp(10.f) 수치는 프레임당 최대 이동 거리로 넉넉히 잡아야 합니다. 예를 들어 20.f)
            FVector FinalPush = Mob->PendingOverlapPush.GetClampedToMaxSize(20.0f);

            // 1. 최종 위치 계산 (현재 캐시된 위치 + 최종 이동/밀림량)
            FVector FinalLocation = Mob->CachedLocation + FinalPush;

            // ★★★ 2. 궁극의 합배송: 위치와 회전을 동시에 엔진에 던짐! ★★★
            Mob->SetActorLocationAndRotation(
                FinalLocation,
                Mob->PendingRotation, // 아까 MoveToPlayer에서 담아둔 회전값
                false,
                nullptr,
                ETeleportType::TeleportPhysics
            );

            //Mob->AddActorWorldOffset(FinalPush, false, nullptr, ETeleportType::TeleportPhysics);

            // 3. 다음 프레임을 위해 위치 캐시 갱신
            Mob->CachedLocation = FinalLocation;
        }

        // 장바구니 리셋은 무조건!
        Mob->PendingOverlapPush = FVector::ZeroVector;
    }

    LastFrameTouchedCells = MoveTemp(ThisFrameCells);
}

void UDEMonsterUpdateComponent::ResolveMonsterOverlap(ADEMonsterBase* A, ADEMonsterBase* B,
    const FVector& PosA, const FVector& PosB)
{
    FVector Delta = FVector(PosB.X - PosA.X, PosB.Y - PosA.Y, 0.f);
    const float DistSq = Delta.SizeSquared();

    // [최적화] 함수 호출(GetCollisionRadius) 다 빼고 아까 저장해둔 변수로 덧셈!
    const float MinDist = A->CachedRadius + B->CachedRadius + 2.0f;
    const float MinDistSq = MinDist * MinDist;

    if (DistSq >= MinDistSq) return;

    if (DistSq < KINDA_SMALL_NUMBER)
    {
        FVector Nudge = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 0.f).GetSafeNormal();
        A->PendingOverlapPush -= Nudge;
        B->PendingOverlapPush += Nudge;
        return;
    }

    /*const float Dist = FMath::Sqrt(DistSq);
    const float Penetration = MinDist - Dist;
    const FVector PushDir = Delta / Dist;
    FVector PushOffset = PushDir * (Penetration * 0.05f);*/

    // [개선된 최적화 코드]
// 1. 역제곱근 (1 / Sqrt(x)) 을 하드웨어 가속으로 순식간에 구함
    const float InvDist = FMath::InvSqrt(DistSq);

    // 2. 수학 공식: x * (1 / Sqrt(x)) = Sqrt(x) 
    const float Dist = DistSq * InvDist;

    // 3. 나눗셈(/) 대신 곱셈(*) 사용! CPU가 기립박수 칩니다.
    const FVector PushDir = Delta * InvDist;
    const float Penetration = MinDist - Dist;
    FVector PushOffset = PushDir * (Penetration * 0.05f);

    A->PendingOverlapPush -= PushOffset;
    B->PendingOverlapPush += PushOffset;

    // 넉백 최적화 로직은 완벽하므로 그대로 유지! (SizeSquared 최고입니다)
    const float AKnockSq = A->KnockbackVelocity.SizeSquared();
    const float BKnockSq = B->KnockbackVelocity.SizeSquared(); // ← 여기로 올려야 함

    if (AKnockSq > 1.f)
    {
        B->ApplyKnockback(PushDir, FMath::Sqrt(AKnockSq) * ChainKnockbackTransfer);
        A->KnockbackVelocity *= 0.5f;
    }
    if (BKnockSq > 1.f)
    {
        A->ApplyKnockback(-PushDir, FMath::Sqrt(BKnockSq) * ChainKnockbackTransfer);
        B->KnockbackVelocity *= 0.5f;
    }
}

void UDEMonsterUpdateComponent::ResolvePlayerPush(ADEMonsterBase* Mob, const FVector& PlayerLocation, const FVector& MobLocation, float PlayerRadius)
{
    if (!Mob || !Player) return;

    //FVector Delta = MobLocation - Player->GetActorLocation();
    FVector Delta = MobLocation - PlayerLocation;
    Delta.Z = 0.f;

    const float DistSq = Delta.SizeSquared();
    const float MinDist = PlayerRadius + Mob->CachedRadius;
    const float MinDistSq = MinDist * MinDist;

    if (DistSq >= MinDistSq || DistSq < KINDA_SMALL_NUMBER)
        return;

    const float Dist = FMath::Sqrt(DistSq);
    const FVector PushDir = Delta / Dist;
    const float Penetration = (MinDist - Dist) - 2.0f;

    Mob->PendingOverlapPush += (PushDir * Penetration);

    //Mob->AddActorWorldOffset(PushDir * Penetration, false);
    //if (!Mob || !Player) return;

    //FVector Delta = Mob->GetActorLocation() - Player->GetActorLocation();
    //Delta.Z = 0.f;

    //const float DistSq = Delta.SizeSquared();
    //const float MinDist = Player->GetCapsuleHalfRadius() + Mob->GetCollisionRadius();
    //const float MinDistSq = MinDist * MinDist;

    //if (DistSq >= MinDistSq || DistSq < KINDA_SMALL_NUMBER)
    //    return;

    //const float Dist = FMath::Sqrt(DistSq);
    //const FVector PushDir = Delta / Dist;
    //const float Penetration = (MinDist - Dist) - 2.0f; // 2유닛 버퍼 (Overlap 이벤트 유지용)

    //Mob->AddActorWorldOffset(PushDir * Penetration, true);
}
