// Fill out your copyright notice in the Description page of Project Settings.


#include "DEMonsterQuerySubsystem.h"
#include "DEMonsterBase.h"
#include "Algo/Sort.h"

void UDEMonsterQuerySubsystem::Deinitialize()
{
    Grid.Empty();
    OccupiedCells.Empty();
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

void UDEMonsterQuerySubsystem::QueryCircle(const FVector& Center, float Radius, TArray<ADEMonsterBase*>& OutMonsters) const
{
    const FVector2D Center2D(Center);
    ForEachMonsterInBounds(Center2D - Radius, Center2D + Radius, [&](ADEMonsterBase* Mob)
    {
        const float HitDist = Radius + Mob->CachedRadius;
        if (FVector2D::DistSquared(Center2D, FVector2D(Mob->CachedLocation)) <= HitDist * HitDist)
        {
            OutMonsters.Add(Mob);
        }
    });
}

ADEMonsterBase* UDEMonsterQuerySubsystem::QueryNearest(const FVector& Center, float MaxRadius) const
{
    const FVector2D Center2D(Center);
    ADEMonsterBase* Nearest = nullptr;
    float NearestDistSq = TNumericLimits<float>::Max();

    auto Consider = [&](ADEMonsterBase* Mob)
    {
        const float DistSq = FVector2D::DistSquared(Center2D, FVector2D(Mob->CachedLocation));
        if (DistSq < NearestDistSq)
        {
            NearestDistSq = DistSq;
            Nearest = Mob;
        }
    };

    if (MaxRadius < 0.0f)
    {
        for (const FIntPoint& Cell : OccupiedCells)
        {
            for (ADEMonsterBase* Mob : Grid.FindChecked(Cell))
            {
                if (Mob && Mob->IsAlive()) Consider(Mob);
            }
        }
        return Nearest;
    }

    ForEachMonsterInBounds(Center2D - MaxRadius, Center2D + MaxRadius, [&](ADEMonsterBase* Mob)
    {
        const float HitDist = MaxRadius + Mob->CachedRadius;
        if (FVector2D::DistSquared(Center2D, FVector2D(Mob->CachedLocation)) <= HitDist * HitDist)
        {
            Consider(Mob);
        }
    });
    return Nearest;
}

void UDEMonsterQuerySubsystem::QuerySegment(const FVector& Start, const FVector& End, float Radius, TArray<ADEMonsterBase*>& OutMonsters) const
{
    const FVector Start2D(Start.X, Start.Y, 0.0f);
    const FVector End2D(End.X, End.Y, 0.0f);
    const FVector2D Min(FMath::Min(Start.X, End.X) - Radius, FMath::Min(Start.Y, End.Y) - Radius);
    const FVector2D Max(FMath::Max(Start.X, End.X) + Radius, FMath::Max(Start.Y, End.Y) + Radius);
    const int32 FirstNew = OutMonsters.Num();

    ForEachMonsterInBounds(Min, Max, [&](ADEMonsterBase* Mob)
    {
        const FVector MobLoc2D(Mob->CachedLocation.X, Mob->CachedLocation.Y, 0.0f);
        const float HitDist = Radius + Mob->CachedRadius;
        if (FMath::PointDistToSegmentSquared(MobLoc2D, Start2D, End2D) <= HitDist * HitDist)
        {
            OutMonsters.Add(Mob);
        }
    });

    // 관통 횟수가 정해진 발사체는 먼저 닿는 적부터 맞아야 하므로 Start에서 가까운 순으로 정렬한다
    const FVector2D StartXY(Start);
    TArrayView<ADEMonsterBase*> NewResults(OutMonsters.GetData() + FirstNew, OutMonsters.Num() - FirstNew);
    Algo::Sort(NewResults, [&StartXY](const ADEMonsterBase* A, const ADEMonsterBase* B)
    {
        return FVector2D::DistSquared(StartXY, FVector2D(A->CachedLocation)) < FVector2D::DistSquared(StartXY, FVector2D(B->CachedLocation));
    });
}
