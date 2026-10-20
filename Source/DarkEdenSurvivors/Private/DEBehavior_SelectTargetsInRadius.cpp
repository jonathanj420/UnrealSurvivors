// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SelectTargetsInRadius.h"
#include "DEMonsterBase.h"
#include "DEMonsterQuerySubsystem.h"
#include "Engine/World.h"
#include "DESkillContext.h"
#include "DEGameMode_Stage.h"
#include "DrawDebugHelpers.h"
#include "DEMonsterSpawnManager.h"
#include "Kismet/GameplayStatics.h"

void UDEBehavior_SelectTargetsInRadius::Execute(FDESkillContext& Context)
{
    if (!Context.Instigator) return;
    UWorld* World = Context.Instigator->GetWorld();

    // 맵 전체 검색: 스폰 매니저가 들고 있는 활성 몬스터를 그대로 쓴다
    if (bTargetAllMonsters)
    {
        Context.Targets.Reset();
        if (ADEGameMode_Stage* GameMode = Cast<ADEGameMode_Stage>(UGameplayStatics::GetGameMode(World)))
        {
            if (ADEMonsterSpawnManager* SpawnManager = GameMode->GetMonsterSpawnManager())
            {
                Context.Targets.Append(SpawnManager->GetActiveMonsters());
            }
        }
        return;
    }

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

    Context.Targets.Reset();

    // 1. 결과 담을 그릇 준비
    TArray<AActor*> RawResults; // 중복 허용 그릇
    TSet<AActor*> UniqueResults; // 중복 방지 그릇

    const float FinalRadius = ResolveOverride(Radius, Context.Radius);

    const UDEMonsterQuerySubsystem* Query = World->GetSubsystem<UDEMonsterQuerySubsystem>();
    if (!Query) return;

    // 2. 중심점마다 검색 (몬스터만 반환하므로 Instigator는 자연히 제외된다)
    TArray<ADEMonsterBase*> Found;
    for (const FVector& Center : CenterPoints)
    {
        Found.Reset();
        Query->QueryCircle(Center, FinalRadius, Found);

        for (ADEMonsterBase* Monster : Found)
        {
            if (bAllowOverlap)
            {
                // 옵션 켜짐: 여러 중심점에 걸린 몬스터는 그만큼 중복으로 담는다 (A, A, A...)
                RawResults.Add(Monster);
            }
            else
            {
                // 옵션 꺼짐: Set으로 중복 제거 (A)
                UniqueResults.Add(Monster);
            }
        }
    }

    // 3. 결과 갱신
    //Context.Targets.Empty();

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


}
