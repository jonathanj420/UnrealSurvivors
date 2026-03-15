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
    if (!Player || ActiveMonsters.Num() == 0)
        return;

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

        if (Mob->IsStunned())
            continue;

        Mob->MoveToPlayer(DeltaTime, PlayerLocation);
        Mob->ExecuteAttackLogic(CurrentWorldTime);

        ResolvePlayerPush(Mob);
    }

    //for (ADEMonsterBase* Mob : ActiveMonsters)
    //{
    //    if (!Mob) continue;

    //    Mob->UpdateKnockback(DeltaTime);

    //    if (Mob->IsStunned())
    //        continue;

    //    Mob->MoveToPlayer(DeltaTime, PlayerLocation);
    //    Mob->ExecuteAttackLogic(CurrentWorldTime);
    //    ResolvePlayerPush(Mob);
    //}
}

// -------------------------------------------------------
// 몬스터 간 겹침 해소 O(n^2) - 추후 공간분할로 개선 여지 있음
// -------------------------------------------------------
void UDEMonsterUpdateComponent::ResolveOverlaps(TArray<ADEMonsterBase*>& ActiveMonsters)
{
    // ★ 핵심: 메모리를 파괴하지 않고 알맹이만 비움 (가비지 컬렉션 방지)
    SpatialGrid.Reset();

    // -------------------------------------------------------------
    // [Phase 1] 몬스터들을 위치에 따라 바둑판(Grid) 방에 배정
    // -------------------------------------------------------------
    for (ADEMonsterBase* Mob : ActiveMonsters)
    {
        if (!Mob) continue;

        FVector Loc = Mob->GetActorLocation();

        // 내 실제 좌표를 CellSize로 나눠서 내가 들어갈 방 번호(X, Y) 계산
        FIntPoint CellIndex(
            FMath::FloorToInt(Loc.X / CellSize),
            FMath::FloorToInt(Loc.Y / CellSize)
        );

        SpatialGrid.FindOrAdd(CellIndex).Add(Mob);
    }

    // -------------------------------------------------------------
    // [Phase 2] 내 주변 9개 방에 있는 놈들하고만 충돌 검사
    // -------------------------------------------------------------
    for (ADEMonsterBase* MobA : ActiveMonsters)
    {
        if (!MobA) continue;

        FVector LocA = MobA->GetActorLocation();
        FIntPoint CellA(
            FMath::FloorToInt(LocA.X / CellSize),
            FMath::FloorToInt(LocA.Y / CellSize)
        );

        // 내 주변 8방향 + 내 방 = 총 9개 방 탐색
        for (int32 dx = -1; dx <= 1; ++dx)
        {
            for (int32 dy = -1; dy <= 1; ++dy)
            {
                FIntPoint NeighborCell(CellA.X + dx, CellA.Y + dy);

                // 이웃 방에 몬스터들이 있다면?
                if (TArray<ADEMonsterBase*>* CellMonsters = SpatialGrid.Find(NeighborCell))
                {
                    for (ADEMonsterBase* MobB : *CellMonsters)
                    {
                        // ★ 포인터 메모리 주소 비교 기법 (나 자신 제외 & 중복 검사 차단)
                        if (MobA < MobB)
                        {
                            ResolveMonsterOverlap(MobA, MobB);
                        }
                    }
                }
            }
        }
    }
}

void UDEMonsterUpdateComponent::ResolveMonsterOverlap(ADEMonsterBase* A, ADEMonsterBase* B)
{
    if (!A || !B) return;

    const FVector PosA = A->GetActorLocation();
    const FVector PosB = B->GetActorLocation();

    const FVector FlatA = FVector(PosA.X, PosA.Y, 0.f);
    const FVector FlatB = FVector(PosB.X, PosB.Y, 0.f);

    FVector Delta = FlatB - FlatA;
    float Dist = Delta.Size();

    const float MinDist = A->GetCollisionRadius() + B->GetCollisionRadius() + 2.0f;

    if (Dist < KINDA_SMALL_NUMBER)
    {
        FVector Nudge = FVector(FMath::FRandRange(-1.f, 1.f), FMath::FRandRange(-1.f, 1.f), 0.f).GetSafeNormal();
        A->AddActorWorldOffset(-Nudge, false);
        B->AddActorWorldOffset(Nudge, false);
        return;
    }

    if (Dist >= MinDist)
        return;

    const float Penetration = MinDist - Dist;
    const FVector PushDir = Delta / Dist;

    A->AddActorWorldOffset(-PushDir * (Penetration * 0.5f), false);
    B->AddActorWorldOffset(PushDir * (Penetration * 0.5f), false);

    // 체인 넉백 전파
    const float AKnockMag = A->KnockbackVelocity.Size();
    if (AKnockMag > 1.f)
    {
        B->ApplyKnockback(PushDir, AKnockMag * ChainKnockbackTransfer);
        A->KnockbackVelocity *= 0.5f;
    }

    const float BKnockMag = B->KnockbackVelocity.Size();
    if (BKnockMag > 1.f)
    {
        A->ApplyKnockback(-PushDir, BKnockMag * ChainKnockbackTransfer);
        B->KnockbackVelocity *= 0.5f;
    }
}

void UDEMonsterUpdateComponent::ResolvePlayerPush(ADEMonsterBase* Mob)
{
    if (!Mob || !Player) return;

    FVector Delta = Mob->GetActorLocation() - Player->GetActorLocation();
    Delta.Z = 0.f;

    const float DistSq = Delta.SizeSquared();
    const float MinDist = Player->GetCapsuleHalfRadius() + Mob->GetCollisionRadius();
    const float MinDistSq = MinDist * MinDist;

    if (DistSq >= MinDistSq || DistSq < KINDA_SMALL_NUMBER)
        return;

    const float Dist = FMath::Sqrt(DistSq);
    const FVector PushDir = Delta / Dist;
    const float Penetration = (MinDist - Dist) - 2.0f; // 2유닛 버퍼 (Overlap 이벤트 유지용)

    Mob->AddActorWorldOffset(PushDir * Penetration, true);
}
