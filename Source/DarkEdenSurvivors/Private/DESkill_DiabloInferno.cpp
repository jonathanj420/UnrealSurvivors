// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_DiabloInferno.h"
#include "DEProjectile_DiabloInferno.h"
#include "DEBehavior_FireProjectile.h"

UDESkill_DiabloInferno::UDESkill_DiabloInferno()
{
    ProjectileClass = ADEProjectile_DiabloInferno::StaticClass();
    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Custom/Inferno.Inferno")
    );

    if (SoundObj.Succeeded())
    {
        FireSound = SoundObj.Object;
    }
}

void UDESkill_DiabloInferno::InitBehaviors()
{
    Super::InitBehaviors();

    // [전방 발사 비헤이비어]
    UDEBehavior_FireProjectile* FireAction = NewObject<UDEBehavior_FireProjectile>(this);
    FireAction->ProjectileClass = this->ProjectileClass;
    FireAction->FireSound = this->FireSound;
    FireAction->BurstInterval = 0.04f;
    FireAction->FireConeAngle = 60.0f;
    FireAction->AngleOffset = 0.0f; // 정면!
    Behaviors.Add(FireAction);

    // [후방 발사 비헤이비어]
    UDEBehavior_FireProjectile* FireActionRear = NewObject<UDEBehavior_FireProjectile>(this);
    FireActionRear->ProjectileClass = this->ProjectileClass;
    FireActionRear->FireSound = this->FireSound;
    FireActionRear->BurstInterval = 0.04f;
    FireActionRear->FireConeAngle = 60.0f;
    FireActionRear->AngleOffset = 180.0f; // 후면! (180도 회전)
    Behaviors.Add(FireActionRear);

}

void UDESkill_DiabloInferno::Activate()
{
    Super::Activate();
}
