// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_Inferno.h"
#include "DEProjectile_Inferno.h"
#include "DEBehavior_FireProjectile.h"


UDESkill_Inferno::UDESkill_Inferno()
{
    ProjectileClass = ADEProjectile_Inferno::StaticClass();
    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Custom/SC_Inferno.SC_Inferno")
    );

    if (SoundObj.Succeeded())
    {
        FireSound = SoundObj.Object;
    }
}

void UDESkill_Inferno::InitBehaviors()
{
    Super::InitBehaviors();
    UDEBehavior_FireProjectile* FireAction = NewObject<UDEBehavior_FireProjectile>(this);
    FireAction->ProjectileClass = this->ProjectileClass;
    FireAction->FireSound = this->FireSound;
    FireAction->BurstInterval = 0.04f;
    FireAction->FireConeAngle = 60.0f;

    Behaviors.Add(FireAction);

}
