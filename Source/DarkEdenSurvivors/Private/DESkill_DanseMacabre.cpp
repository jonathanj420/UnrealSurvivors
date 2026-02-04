// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_DanseMacabre.h"
#include "DEBehavior_SpawnOrbit.h"
#include "DEAOE_Orbit_DanseMacabre.h"

UDESkill_DanseMacabre::UDESkill_DanseMacabre()
{
    //AOEClass = ADEAOE_Orbit_DanseMacabre::StaticClass();

}

void UDESkill_DanseMacabre::InitBehaviors()
{

	Super::InitBehaviors();

	// --------------------------------------------------
	// SpawnOrbit : 성서형 Orbit 스폰 전담
	// --------------------------------------------------
	UDEBehavior_SpawnOrbit* SpawnOrbit =
		NewObject<UDEBehavior_SpawnOrbit>(this);

	if (SpawnOrbit)
	{
		// ★ 핵심
		// Orbit 스킬이 어떤 AOE를 쓰는지는
		// Skill이 아니라 Behavior가 안다
		SpawnOrbit->OrbitAOEClass =
			ADEAOE_Orbit_DanseMacabre::StaticClass();

		Behaviors.Add(SpawnOrbit);
	}
}
