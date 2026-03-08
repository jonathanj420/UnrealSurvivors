// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_Executioner.h"
#include "DEProjectile_AcidBolt.h"
#include "DEBehavior_SelectTargetsInRadius.h"
#include "DEBehavior_FilterTargets.h"
#include "DEBehavior_FireProjectile.h"

UDESkill_Executioner::UDESkill_Executioner()
{
	ProjectileClass = ADEProjectile_AcidBolt::StaticClass();
	static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
		TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Custom/AcidBolt1.AcidBolt1")
	);

	if (SoundObj.Succeeded())
	{
		FireSound = SoundObj.Object;
	}

}

void UDESkill_Executioner::InitBehaviors()
{
	Super::InitBehaviors();

	UDEBehavior_SelectTargetsInRadius* Targeting = NewObject<UDEBehavior_SelectTargetsInRadius>(this);
	Targeting->Radius = 800.0f;
	Targeting->OriginType = ESearchOrigin::Instigator;
	Behaviors.Add(Targeting);
	
	UDEBehavior_FilterTargets* Filtering = NewObject<UDEBehavior_FilterTargets>(this);
	Filtering->TargetCount = 1;
	Filtering->FilterType = ETargetFilterType::Nearest;
	Behaviors.Add(Filtering);


	// 2. [발사 행동] 찾은 적 방향을 기준으로 퍼지게 쏴라
	UDEBehavior_FireProjectile* FireProjectile = NewObject<UDEBehavior_FireProjectile>(this);
	FireProjectile->ProjectileClass = this->ProjectileClass;
	FireProjectile->FireSound = this->FireSound;
	Behaviors.Add(FireProjectile);

}
