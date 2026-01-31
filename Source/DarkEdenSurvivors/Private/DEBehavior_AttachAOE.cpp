// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_AttachAOE.h"
#include "DESkillContext.h"
#include "DESimpleAOEBase.h"
#include "GameFramework/Actor.h"


void UDEBehavior_AttachAOE::Execute(FDESkillContext& Context)
{
    const int32 RequestCount = Context.AOERequests.Num();
    const int32 SpawnedCount = Context.SpawnedAOEs.Num();

    if (RequestCount == 0 || SpawnedCount == 0)
    {
        return;
    }

    const int32 Count = FMath::Min(RequestCount, SpawnedCount);

    for (int32 i = 0; i < Count; ++i)
    {
        const FAOERequest& Request = Context.AOERequests[i];
        ADESimpleAOEBase* AOE = Context.SpawnedAOEs[i].Get();

        if (!AOE || !Request.bAttach || !Request.AttachTarget)
        {
            continue;
        }

        AOE->AttachToActor(
            Request.AttachTarget,
            FAttachmentTransformRules::SnapToTargetNotIncludingScale
        );
    }

}
