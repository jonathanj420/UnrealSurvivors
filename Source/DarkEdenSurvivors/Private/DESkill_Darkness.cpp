// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_Darkness.h"
// Behaviors
#include "DEBehavior_Prepare_PlayerAura.h"
#include "DEBehavior_SpawnAOE.h"
#include "DEBehavior_AttachAOE.h"
// AOE
#include "DEAOE_Darkness.h"

// Context
#include "DESkillContext.h"
UDESkill_Darkness::UDESkill_Darkness()
{
	AOEClass = ADEAOE_Darkness::StaticClass();

}

void UDESkill_Darkness::InitBehaviors()
{
	Super::InitBehaviors();

	UDEBehavior_Prepare_PlayerAura* Prepare = NewObject<UDEBehavior_Prepare_PlayerAura>(this);

	if (Prepare)
	{
		Prepare->AuraAOEClass = AOEClass;
		Prepare->AuraAOEKey = TEXT("Darkness");
		Behaviors.Add(Prepare);

	}

	Behaviors.Add(NewObject<UDEBehavior_SpawnAOE>(this));

	// 3. Attach : 플레이어에게 부착
	Behaviors.Add(NewObject<UDEBehavior_AttachAOE>(this));
}
