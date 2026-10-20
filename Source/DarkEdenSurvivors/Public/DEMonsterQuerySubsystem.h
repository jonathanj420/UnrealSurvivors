// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GenericTeamAgentInterface.h"
#include "DEMonsterQuerySubsystem.generated.h"

class ADEMonsterBase;
class ADECharacterBase;

/**
 * 몬스터 공간 분할 그리드와 스킬 타겟 쿼리.
 * 그리드는 UDEMonsterUpdateComponent가 매 프레임 RebuildGrid로 채우고, 몬스터 겹침 해소와 스킬 타겟 쿼리가 같이 쓴다.
 * 스킬 판정이 물리 씬을 거치지 않게 해서 몬스터 콜리전을 끌 수 있도록 하는 것이 목적이다.
 *
 * 타겟 쿼리는 Querier(스킬 시전자)와 적대인 대상만 반환한다 (DETeam). 몬스터는 그리드에서, 캐릭터(플레이어, 마스터)는
 * 등록 목록을 선형으로 본다. Querier에 팀이 없으면 아무것도 반환하지 않는다.
 * Z를 무시하고 XY 평면에서 판정한다. 대상은 원(몬스터는 CachedRadius, 캐릭터는 캡슐 반지름)으로 보고, 조금이라도 겹치면 포함한다.
 * 살아 있는 대상만 반환한다. 결과 배열은 비우지 않고 뒤에 추가한다.
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEMonsterQuerySubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Deinitialize() override;

    // 몬스터의 CachedLocation 기준으로 그리드를 다시 채운다.
    // MaxMoveAfterRebuild: 다음 Rebuild 전까지 몬스터가 움직일 수 있는 최대 거리. 쿼리 범위를 이만큼 넓혀 셀 경계 누락을 막는다
    void RebuildGrid(const TArray<ADEMonsterBase*>& Monsters, float MaxMoveAfterRebuild);

    // 원 안에 걸치는 적대 대상
    void QueryCircle(const AActor* Querier, const FVector& Center, float Radius, TArray<AActor*>& OutTargets) const;

    // 가장 가까운 적대 대상. MaxRadius < 0이면 거리 제한 없음
    AActor* QueryNearest(const AActor* Querier, const FVector& Center, float MaxRadius) const;

    // Start에서 End까지 반지름 Radius인 구를 쓸고 지나간 영역(캡슐)에 걸치는 적대 대상, Start에서 가까운 순. 발사체의 이전-현재 위치 판정용
    void QuerySegment(const AActor* Querier, const FVector& Start, const FVector& End, float Radius, TArray<AActor*>& OutTargets) const;

    // 맵 전체의 적대 대상
    void QueryAll(const AActor* Querier, TArray<AActor*>& OutTargets) const;

    // 캐릭터는 BeginPlay/EndPlay에서 등록한다
    void RegisterCharacter(ADECharacterBase* Character);
    void UnregisterCharacter(ADECharacterBase* Character);

    // 몬스터 겹침 해소용 셀 접근
    const TSet<FIntPoint>& GetOccupiedCells() const { return OccupiedCells; }
    const TArray<ADEMonsterBase*>* FindCell(const FIntPoint& Cell) const;

private:
    FIntPoint ToCell(const FVector& Location) const;

    // Min~Max 영역(쿼리 패딩 포함)에 걸치는 셀의 살아 있는 몬스터마다 Func 호출
    template <typename FuncType>
    void ForEachMonsterInBounds(const FVector2D& Min, const FVector2D& Max, FuncType&& Func) const;

    // QuerierTeam과 적대인 살아 있는 대상마다 Func(AActor*, 위치 XY, 반지름) 호출. Bounds가 null이면 맵 전체.
    // 캐릭터는 수가 적어 Bounds로 거르지 않는다 (호출하는 쿼리가 거리로 판정한다)
    template <typename FuncType>
    void ForEachHostile(FGenericTeamId QuerierTeam, const FBox2D* Bounds, FuncType&& Func) const;

    float CellSize = 200.0f;

    // 반지름이 가장 큰 몬스터 + 다음 Rebuild까지 이동량. 쿼리 영역을 이만큼 넓힌다
    float QueryPadding = 0.0f;

    // 몬스터는 풀링되어 Destroy되지 않으므로 raw 포인터를 둔다. 살아 있는지는 쿼리 시점에 확인한다
    TMap<FIntPoint, TArray<ADEMonsterBase*>> Grid;

    // 이번 Rebuild에서 몬스터가 들어간 셀. 다음 Rebuild 때 이 셀들만 비운다
    TSet<FIntPoint> OccupiedCells;

    // 플레이어와 마스터. 몇 개뿐이라 그리드에 넣지 않는다
    TArray<TWeakObjectPtr<ADECharacterBase>> Characters;
};
