// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_GreyDarkness.h"
#include "DEBehavior_Prepare_PlayerAura.h"
#include "DEBehavior_SpawnAOE.h"
#include "DEBehavior_AttachAOE.h"
#include "DEAOE_GreyDarkness.h"
UDESkill_GreyDarkness::UDESkill_GreyDarkness()
{
	AOEClass = ADEAOE_GreyDarkness::StaticClass();
}

void UDESkill_GreyDarkness::InitBehaviors()
{
	Super::InitBehaviors();

	UDEBehavior_Prepare_PlayerAura* Prepare = NewObject<UDEBehavior_Prepare_PlayerAura>(this);

	if (Prepare)
	{
		Prepare->AuraAOEClass = AOEClass;
		Prepare->AuraAOEKey = TEXT("GreyDarkness");
		Behaviors.Add(Prepare);

	}

	Behaviors.Add(NewObject<UDEBehavior_SpawnAOE>(this));

	// 3. Attach : 플레이어에게 부착
	Behaviors.Add(NewObject<UDEBehavior_AttachAOE>(this));
}
