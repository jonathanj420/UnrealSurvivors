// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_DeathSentence.h"
#include "DEProjectile_Boomerang.h"
#include "DEBehavior_SelectTargetsInRadius.h"
#include "DEBehavior_FilterTargets.h"
#include "DEBehavior_FireProjectile.h"

UDESkill_DeathSentence::UDESkill_DeathSentence()
{


	static ConstructorHelpers::FClassFinder<ADEProjectile_Boomerang> ProjectileBP(
		TEXT("/Game/DarkEden/Blueprint/SkillProjectiles/DEProjectile_Mortal.DEProjectile_Mortal_C")
	);

	if (ProjectileBP.Succeeded())
	{
		ProjectileClass = ProjectileBP.Class;
	}


	static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
		TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Custom/AcidBolt1.AcidBolt1")
	);

	if (SoundObj.Succeeded())
	{
		FireSound = SoundObj.Object;
	}

}

void UDESkill_DeathSentence::InitBehaviors()
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
	FireProjectile->BurstInterval = 0.1f;
	FireProjectile->FireConeAngle = 10.0f;
	Behaviors.Add(FireProjectile);

}
