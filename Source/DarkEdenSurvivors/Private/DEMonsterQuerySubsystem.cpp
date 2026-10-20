// Fill out your copyright notice in the Description page of Project Settings.


#include "DEMonsterQuerySubsystem.h"
#include "DEMonsterBase.h"
#include "DECharacterBase.h"
#include "DETeam.h"
#include "Components/CapsuleComponent.h"
#include "Algo/Sort.h"

void UDEMonsterQuerySubsystem::Deinitialize()
{
    Grid.Empty();
    OccupiedCells.Empty();
    Characters.Empty();
    Super::Deinitialize();
}

FIntPoint UDEMonsterQuerySubsystem::ToCell(const FVector& Location) const
{
    return FIntPoint(FMath::FloorToInt(Location.X / CellSize), FMath::FloorToInt(Location.Y / CellSize));
}

void UDEMonsterQuerySubsystem::RebuildGrid(const TArray<ADEMonsterBase*>& Monsters, float MaxMoveAfterRebuild)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UDEMonsterQuerySubsystem::RebuildGrid);

    // 이전에 쓴 셀만 비운다 (Reset은 메모리 유지)
    for (const FIntPoint& Cell : OccupiedCells)
    {
        if (TArray<ADEMonsterBase*>* Arr = Grid.Find(Cell))
        {
            Arr->Reset();
        }
    }
    OccupiedCells.Reset();

    float MaxRadius = 0.0f;
    for (ADEMonsterBase* Mob : Monsters)
    {
        if (!Mob) continue;

        const FIntPoint Cell = ToCell(Mob->CachedLocation);
        Grid.FindOrAdd(Cell).Add(Mob);
        OccupiedCells.Add(Cell);
        MaxRadius = FMath::Max(MaxRadius, Mob->CachedRadius);
    }

    QueryPadding = MaxRadius + MaxMoveAfterRebuild;
}

const TArray<ADEMonsterBase*>* UDEMonsterQuerySubsystem::FindCell(const FIntPoint& Cell) const
{
    return Grid.Find(Cell);
}

template <typename FuncType>
void UDEMonsterQuerySubsystem::ForEachMonsterInBounds(const FVector2D& Min, const FVector2D& Max, FuncType&& Func) const
{
    const FIntPoint MinCell = ToCell(FVector(Min.X - QueryPadding, Min.Y - QueryPadding, 0.0f));
    const FIntPoint MaxCell = ToCell(FVector(Max.X + QueryPadding, Max.Y + QueryPadding, 0.0f));

    // 영역이 점유 셀 수보다 넓으면 점유 셀만 도는 쪽이 싸다 (맵 전체 범위 쿼리 등)
    const int64 BoundsCellCount = int64(MaxCell.X - MinCell.X + 1) * int64(MaxCell.Y - MinCell.Y + 1);
    if (BoundsCellCount > OccupiedCells.Num())
    {
        for (const FIntPoint& Cell : OccupiedCells)
        {
            if (Cell.X < MinCell.X || Cell.X > MaxCell.X || Cell.Y < MinCell.Y || Cell.Y > MaxCell.Y) continue;
            for (ADEMonsterBase* Mob : Grid.FindChecked(Cell))
            {
                if (Mob && Mob->IsAlive()) Func(Mob);
            }
        }
        return;
    }

    for (int32 X = MinCell.X; X <= MaxCell.X; ++X)
    {
        for (int32 Y = MinCell.Y; Y <= MaxCell.Y; ++Y)
        {
            const TArray<ADEMonsterBase*>* Cell = Grid.Find(FIntPoint(X, Y));
            if (!Cell) continue;
            for (ADEMonsterBase* Mob : *Cell)
            {
                if (Mob && Mob->IsAlive()) Func(Mob);
            }
        }
    }
}

template <typename FuncType>
void UDEMonsterQuerySubsystem::ForEachHostile(FGenericTeamId QuerierTeam, const FBox2D* Bounds, FuncType&& Func) const
{
    if (QuerierTeam == FGenericTeamId::NoTeam) return;

    if (DETeam::AreHostile(QuerierTeam, DETeam::Monster))
    {
        auto MonsterFunc = [&Func](ADEMonsterBase* Mob)
        {
            Func(Mob, FVector2D(Mob->CachedLocation), Mob->CachedRadius);
        };

        if (Bounds)
        {
            ForEachMonsterInBounds(Bounds->Min, Bounds->Max, MonsterFunc);
        }
        else
        {
            for (const FIntPoint& Cell : OccupiedCells)
            {
                for (ADEMonsterBase* Mob : Grid.FindChecked(Cell))
                {
                    if (Mob && Mob->IsAlive()) MonsterFunc(Mob);
                }
            }
        }
    }

    for (const TWeakObjectPtr<ADECharacterBase>& WeakCharacter : Characters)
    {
        ADECharacterBase* Character = WeakCharacter.Get();
        if (!Character || Character->IsDead()) continue;
        if (!DETeam::AreHostile(QuerierTeam, Character->GetGenericTeamId())) continue;

        Func(Character, FVector2D(Character->GetActorLocation()), Character->GetCapsuleComponent()->GetScaledCapsuleRadius());
    }
}

void UDEMonsterQuerySubsystem::QueryCircle(const AActor* Querier, const FVector& Center, float Radius, TArray<AActor*>& OutTargets) const
{
    const FVector2D Center2D(Center);
    const FBox2D Bounds(Center2D - Radius, Center2D + Radius);
    ForEachHostile(DETeam::GetTeam(Querier), &Bounds, [&](AActor* Target, const FVector2D& TargetLoc, float TargetRadius)
    {
        const float HitDist = Radius + TargetRadius;
        if (FVector2D::DistSquared(Center2D, TargetLoc) <= HitDist * HitDist)
        {
            OutTargets.Add(Target);
        }
    });
}

AActor* UDEMonsterQuerySubsystem::QueryNearest(const AActor* Querier, const FVector& Center, float MaxRadius) const
{
    const FVector2D Center2D(Center);
    AActor* Nearest = nullptr;
    float NearestDistSq = TNumericLimits<float>::Max();

    const FBox2D Bounds(Center2D - MaxRadius, Center2D + MaxRadius);
    ForEachHostile(DETeam::GetTeam(Querier), MaxRadius < 0.0f ? nullptr : &Bounds, [&](AActor* Target, const FVector2D& TargetLoc, float TargetRadius)
    {
        const float DistSq = FVector2D::DistSquared(Center2D, TargetLoc);
        if (MaxRadius >= 0.0f)
        {
            const float HitDist = MaxRadius + TargetRadius;
            if (DistSq > HitDist * HitDist) return;
        }
        if (DistSq < NearestDistSq)
        {
            NearestDistSq = DistSq;
            Nearest = Target;
        }
    });
    return Nearest;
}

void UDEMonsterQuerySubsystem::QuerySegment(const AActor* Querier, const FVector& Start, const FVector& End, float Radius, TArray<AActor*>& OutTargets) const
{
    const FVector Start2D(Start.X, Start.Y, 0.0f);
    const FVector End2D(End.X, End.Y, 0.0f);
    const FVector2D StartXY(Start);
    const FBox2D Bounds(
        FVector2D(FMath::Min(Start.X, End.X) - Radius, FMath::Min(Start.Y, End.Y) - Radius),
        FVector2D(FMath::Max(Start.X, End.X) + Radius, FMath::Max(Start.Y, End.Y) + Radius));

    // 관통 횟수가 정해진 발사체는 먼저 닿는 적부터 맞아야 하므로 Start에서 가까운 순으로 정렬한다
    TArray<TPair<float, AActor*>, TInlineAllocator<16>> Hits;
    ForEachHostile(DETeam::GetTeam(Querier), &Bounds, [&](AActor* Target, const FVector2D& TargetLoc, float TargetRadius)
    {
        const float HitDist = Radius + TargetRadius;
        if (FMath::PointDistToSegmentSquared(FVector(TargetLoc, 0.0f), Start2D, End2D) <= HitDist * HitDist)
        {
            Hits.Emplace(FVector2D::DistSquared(StartXY, TargetLoc), Target);
        }
    });

    Algo::SortBy(Hits, &TPair<float, AActor*>::Key);
    for (const TPair<float, AActor*>& Hit : Hits)
    {
        OutTargets.Add(Hit.Value);
    }
}

void UDEMonsterQuerySubsystem::QueryAll(const AActor* Querier, TArray<AActor*>& OutTargets) const
{
    ForEachHostile(DETeam::GetTeam(Querier), nullptr, [&OutTargets](AActor* Target, const FVector2D&, float)
    {
        OutTargets.Add(Target);
    });
}

void UDEMonsterQuerySubsystem::RegisterCharacter(ADECharacterBase* Character)
{
    if (Character)
    {
        Characters.AddUnique(Character);
    }
}

void UDEMonsterQuerySubsystem::UnregisterCharacter(ADECharacterBase* Character)
{
    Characters.RemoveAll([Character](const TWeakObjectPtr<ADECharacterBase>& Weak)
    {
        return !Weak.IsValid() || Weak.Get() == Character;
    });
}
