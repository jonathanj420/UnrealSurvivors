// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_AcidBolt.h"
#include "DEProjectile_AcidBolt.h"
#include "Kismet/GameplayStatics.h"
#include "DEBehavior_FireProjectileRadial.h"
#include "DEBehavior_SelectNearestTarget.h"
//#include "DEPoolSubsystem.h"
UDESkill_AcidBolt::UDESkill_AcidBolt()
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

void UDESkill_AcidBolt::InitBehaviors()
{
	Super::InitBehaviors();

	// 1. [탐색 행동] 가장 가까운 적을 찾아라
	UDEBehavior_SelectNearestTarget* Targeting = NewObject<UDEBehavior_SelectNearestTarget>(this);
	Targeting->DefaultSearchRadius = 800.f; // 2000 = mob auto repos rad
	Targeting->EnemyTag = TEXT("Enemy");    // 
	Behaviors.Add(Targeting);

	// 2. [발사 행동] 찾은 적 방향을 기준으로 퍼지게 쏴라
	UDEBehavior_FireProjectileRadial* RadialFire = NewObject<UDEBehavior_FireProjectileRadial>(this);
	RadialFire->ProjectileClass = this->ProjectileClass;
	RadialFire->FireSound = this->FireSound;
	RadialFire->ArcAngle = 360.f; // 360도
	Behaviors.Add(RadialFire);
}
