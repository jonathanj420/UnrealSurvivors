// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_Prepare_PlayerAura.h"
#include "GameFramework/Actor.h"
#include "DESkillContext.h"
#include "DESimpleAOEBase.h"

void UDEBehavior_Prepare_PlayerAura::Execute(FDESkillContext& Context)
{
    AActor* Instigator = Context.Instigator;
    if (!Instigator || !AuraAOEClass)
    {
        return;
    }

    FAOERequest Request;
    Request.AOEKey = AuraAOEKey;
    Request.AOEClass = AuraAOEClass;
    Request.SpawnLocation = Instigator->GetActorLocation();
    Request.AttachTarget = Instigator;
    Request.bAttach = true;

    Context.AOERequests.Add(Request);
}
