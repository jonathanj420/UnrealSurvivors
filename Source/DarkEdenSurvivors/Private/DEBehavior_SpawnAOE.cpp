// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SpawnAOE.h"
#include "DEAutoSkillBase.h"          // ActiveSkill 접근용
#include "DESkillContext.h"        // 컨텍스트 정의
#include "DESimpleAOEBase.h"      // 장판 액터 베이스
#include "DEPoolSubsystem.h"       // 풀링 서브시스템
#include "DECharacterBase.h"
#include "Kismet/GameplayStatics.h"
void UDEBehavior_SpawnAOE::Execute(FDESkillContext& Context)
{
    // =========================================================
        // 0. 유효성 체크 및 풀링 매니저 확보
        // =========================================================
    AActor* Instigator = Context.Instigator;
    if (!Instigator || !AOEClass) return;

    UWorld* World = Instigator->GetWorld();
    if (!World) return;

    UDEPoolSubsystem* Pool = World->GetSubsystem<UDEPoolSubsystem>();
    if (!Pool) return;

    UDEAutoSkillBase* SourceSkill = Context.SourceSkill;
    const bool bHasKey = !AOEKey.IsNone();

    // =========================================================
    // 1. 영구 장판(마늘 오라) 중복 방지 및 실시간 스탯 갱신!
    // =========================================================
    // 시전자(Player)에게 붙는 단일 오라이면서 Key가 있는 경우
    if (bHasKey && SourceSkill && SpawnTarget == EAOESpawnTarget::Instigator)
    {
        if (TWeakObjectPtr<ADESimpleAOEBase>* Found = SourceSkill->OwnedAOEMap.Find(AOEKey))
        {
            if (Found->IsValid())
            {
                ADESimpleAOEBase* ExistingAOE = Found->Get();

                // 이미 존재하므로 풀에서 안 꺼내고, 갓 레벨업된 최신 Context만 덮어씌웁니다!
                ExistingAOE->ApplyContext(Context);
                ExistingAOE->ActivateAOE(/*bIsNewSpawn=*/false);

                return; // 스폰은 필요 없으니 여기서 깔끔하게 칼퇴근!
            }
        }
    }

    // =========================================================
    // 2. 스폰 위치 및 부착 대상(AttachActor) 수집
    // =========================================================
    struct FSpawnInfo {
        FVector Location;
        AActor* AttachActor;
    };
    TArray<FSpawnInfo> SpawnInfos;

    switch (SpawnTarget)
    {
    case EAOESpawnTarget::Instigator: // 마늘: 내 위치, 내 몸에 부착
        SpawnInfos.Add({ Instigator->GetActorLocation(), Instigator });
        break;

    case EAOESpawnTarget::AllTargets: // 독오라: 적들 위치, 적들 몸에 부착
        for (AActor* Target : Context.Targets)
        {
            if (Target) SpawnInfos.Add({ Target->GetActorLocation(), Target });
        }
        break;

    case EAOESpawnTarget::CustomLocations: // 성수/눈보라: 계산된 허공/바닥 (부착 없음)
        for (const FVector& Loc : Context.CustomLocations)
        {
            SpawnInfos.Add({ Loc, nullptr });
        }
        break;
    }

    // =========================================================
    // 3. 오브젝트 풀(Pool)에서 꺼내와서 폭풍 스폰!
    // =========================================================
    for (const FSpawnInfo& Info : SpawnInfos)
    {
        // 메모리 할당 없이 풀에서 번개같이 가져오기!
        AActor* RawActor = Pool->GetPooledActor(AOEClass, Info.Location, FRotator::ZeroRotator, false);
        ADESimpleAOEBase* AOE = Cast<ADESimpleAOEBase>(RawActor);

        if (!AOE) continue;

        AOE->SetOwner(Instigator);

        // 부착(Attach) 옵션이 켜져 있고 대상이 존재하면 찰싹!
        if (bAttachToTarget && Info.AttachActor)
        {
            AOE->AttachToActor(Info.AttachActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
        }

        // 스탯 주입 및 활성화
        AOE->ApplyContext(Context);
        AOE->ActivateAOE(/*bIsNewSpawn=*/true);

        // =========================================================
        // 4. 수명 관리를 위해 스킬 본체에 명부 등록
        // =========================================================
        if (bHasKey && SourceSkill && SpawnTarget == EAOESpawnTarget::Instigator)
        {
            SourceSkill->OwnedAOEMap.Add(AOEKey, AOE);
        }

        // 혹시 뒷단 비헤이비어에서 쓸지 모르니 가볍게 기록만 남김
        Context.SpawnedAOEs.Add(AOE);
    }

}
