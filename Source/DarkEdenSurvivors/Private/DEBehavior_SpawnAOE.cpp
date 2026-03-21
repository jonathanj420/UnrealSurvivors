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

    // --------------------------------------------------
     // 0. 유효성 체크
     // --------------------------------------------------
    if (Context.AOERequests.Num() == 0)
    {
        return;
    }

    AActor* Instigator = Context.Instigator;
    if (!Instigator)
    {
        return;
    }

    UWorld* World = Instigator->GetWorld();
    if (!World)
    {
        return;
    }

    UGameInstance* GI = World->GetGameInstance();
    if (!GI)
    {
        return;
    }

    UDEPoolSubsystem* Pool = World->GetSubsystem<UDEPoolSubsystem>();
    if (!Pool)
    {
        return;
    }

    // Player 기준 Persistent 관리
    ADECharacterBase* Player = Cast<ADECharacterBase>(Instigator);
    //UE_LOG(LogTemp, Warning, TEXT("Try Spawn AOE"));
    // --------------------------------------------------
    // 1. AOE 요청 처리
    // --------------------------------------------------
    for (const FAOERequest& Request : Context.AOERequests)
    {
        if (!Request.AOEClass)
        {
            continue;
        }

        ADESimpleAOEBase* AOE = nullptr;
        const bool bHasKey = !Request.AOEKey.IsNone();

        // ----------------------------------------------
        // 1-A. Persistent AOE 재사용 시도
        // ----------------------------------------------
        UDEAutoSkillBase* SourceSkill = Context.SourceSkill;
        if (bHasKey && SourceSkill)
        {
            if (TWeakObjectPtr<ADESimpleAOEBase>* Found =
                SourceSkill->OwnedAOEMap.Find(Request.AOEKey))
            {
                if (Found->IsValid())
                {
                    AOE = Found->Get();
                    //UE_LOG(LogTemp, Warning, TEXT("Persistent AOE"));
                }
            }
        }

        // ----------------------------------------------
        // 1-B. 없으면 Pool에서 새로 확보
        // ----------------------------------------------
        bool bIsNewSpawn = false;
        if (!AOE)
        {
            AActor* RawActor = Pool->GetPooledActor(
                Request.AOEClass,
                Request.SpawnLocation,
                FRotator::ZeroRotator,
                /*bAutoActivate=*/false
            );
            UE_LOG(LogTemp, Warning, TEXT("AOE : %s Get Pooled Actor"), *RawActor->GetName());
            AOE = Cast<ADESimpleAOEBase>(RawActor);
            if (!AOE)
            {
                continue;
            }
            UE_LOG(LogTemp, Warning, TEXT("AOE : Casted Successfully"), *AOE->GetName());
            AOE->SetOwner(Instigator);
            bIsNewSpawn = true;
            // Persistent 등록
            if (bHasKey && SourceSkill)
            {
                SourceSkill->OwnedAOEMap.Add(Request.AOEKey, AOE);
            }
        }
        else
        {
            // 재사용 시 위치만 갱신
            AOE->SetActorLocation(Request.SpawnLocation);
            if (AOE->GetOwner() != Instigator)
            {
                AOE->SetOwner(Instigator);

            }
        }

        // ----------------------------------------------
        // 1-C. 핵심: Context 재적용
        // ----------------------------------------------
        AOE->ApplyContext(Context);
        //UE_LOG(LogTemp, Error, TEXT("AOE Context Applied, Damage : %f, Radius %f, Tick : %f"), Context.Damage, Context.Radius, Context.GetValue(TEXT("HitInterval"), 0.5f));
        AOE->ActivateAOE(bIsNewSpawn);
        // ----------------------------------------------
        // 1-D. 결과 기록
        // ----------------------------------------------
        Context.SpawnedAOEs.Add(AOE);
    }

}
