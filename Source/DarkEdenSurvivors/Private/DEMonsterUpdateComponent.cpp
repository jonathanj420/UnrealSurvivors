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

    for (int32 i = ActiveMonsters.Num() - 1; i >= 0; --i)
    {
        ADEMonsterBase* Mob = ActiveMonsters[i];
        if (!Mob) continue;

        Mob->UpdateKnockback(DeltaTime);
        if (Mob->IsStunned()) continue;

        Mob->MoveToPlayer(DeltaTime, PlayerLocation);
        Mob->ExecuteAttackLogic(CurrentWorldTime);

        // [수정] 배열에서 꺼내지 말고, 아까 위에서 구워둔 Mob 주머니(CachedLocation)에서 꺼냄!
        ResolvePlayerPush(Mob, PlayerLocation, Mob->CachedLocation);
    }

}

// -------------------------------------------------------
// 몬스터 간 겹침 해소 O(n^2) - 추후 공간분할로 개선 여지 있음
// -------------------------------------------------------
void UDEMonsterUpdateComponent::ResolveOverlaps(TArray<ADEMonsterBase*>& ActiveMonsters)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UDEMonsterUpdateComponent::ResolveOverlaps);

    const int32 MonsterCount = ActiveMonsters.Num();

    // 1. 그리드 비우기
    for (auto& Pair : SpatialGrid)
    {
        Pair.Value.Reset();
    }

    // 2. 몬스터들을 그리드 셀에 배정
    for (int32 i = 0; i < MonsterCount; ++i)
    {
        ADEMonsterBase* Mob = ActiveMonsters[i];
        if (!Mob) continue;

        const FVector& Loc = Mob->CachedLocation; // 함수 호출 없이 변수만 쏙 빼먹음!
        FIntPoint CellIndex(FMath::FloorToInt(Loc.X / CellSize), FMath::FloorToInt(Loc.Y / CellSize));
        SpatialGrid.FindOrAdd(CellIndex).Add(Mob);
    }

    // 3. 충돌 검사
    for (int32 i = 0; i < MonsterCount; ++i)
    {
        ADEMonsterBase* MobA = ActiveMonsters[i];
        if (!MobA) continue;

        const FVector& LocA = MobA->CachedLocation;
        FIntPoint CellA(FMath::FloorToInt(LocA.X / CellSize), FMath::FloorToInt(LocA.Y / CellSize));

        for (int32 dx = -1; dx <= 1; ++dx)
        {
            for (int32 dy = -1; dy <= 1; ++dy)
            {
                if (TArray<ADEMonsterBase*>* CellMonsters = SpatialGrid.Find(FIntPoint(CellA.X + dx, CellA.Y + dy)))
                {
                    for (ADEMonsterBase* MobB : *CellMonsters)
                    {
                        if (MobA < MobB)
                        {
                            // MobB 위치도 함수 호출 없이 변수로 바로 전달!
                            ResolveMonsterOverlap(MobA, MobB, LocA, MobB->CachedLocation);
                        }
                    }
                }
            }
        }
    }

    // 4. 이동 결재 (엔진의 헛수고 방지)
    for (ADEMonsterBase* Mob : ActiveMonsters)
    {
        if (!Mob) continue;

        //  [부활] 밀어낼 힘이 있을 때'만' 엔진을 깨운다! (이거 진짜 중요합니다)
        if (!Mob->PendingOverlapPush.IsNearlyZero())
        {
            FVector FinalPush = Mob->PendingOverlapPush.GetClampedToMaxSize(10.0f);
            Mob->AddActorWorldOffset(FinalPush, false, nullptr, ETeleportType::TeleportPhysics);
        }

        // 장바구니 리셋은 무조건!
        Mob->PendingOverlapPush = FVector::ZeroVector;
    }


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

    const float Dist = FMath::Sqrt(DistSq);
    const float Penetration = MinDist - Dist;
    const FVector PushDir = Delta / Dist;
    FVector PushOffset = PushDir * (Penetration * 0.05f);

    A->PendingOverlapPush -= PushOffset;
    B->PendingOverlapPush += PushOffset;

    // 넉백 최적화 로직은 완벽하므로 그대로 유지! (SizeSquared 최고입니다)
    const float AKnockSq = A->KnockbackVelocity.SizeSquared();
    if (AKnockSq > 1.f)
    {
        B->ApplyKnockback(PushDir, FMath::Sqrt(AKnockSq) * ChainKnockbackTransfer);
        A->KnockbackVelocity *= 0.5f;
    }

    const float BKnockSq = B->KnockbackVelocity.SizeSquared();
    if (BKnockSq > 1.f)
    {
        A->ApplyKnockback(-PushDir, FMath::Sqrt(BKnockSq) * ChainKnockbackTransfer);
        B->KnockbackVelocity *= 0.5f;
    }
}

void UDEMonsterUpdateComponent::ResolvePlayerPush(ADEMonsterBase* Mob, const FVector& PlayerLocation, const FVector& MobLocation)
{
    if (!Mob || !Player) return;

    //FVector Delta = MobLocation - Player->GetActorLocation();
    FVector Delta = MobLocation - PlayerLocation;
    Delta.Z = 0.f;

    const float DistSq = Delta.SizeSquared();
    const float MinDist = Player->GetCapsuleHalfRadius() + Mob->CachedRadius;
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
