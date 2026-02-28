// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_SanguineBats.h"
#include "DEProjectile_SanguineBat.h"
#include "DEBehavior_FireProjectile.h"


UDESkill_SanguineBats::UDESkill_SanguineBats()
{
    ProjectileClass = ADEProjectile_SanguineBat::StaticClass();
    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Vampire_BloodyKnife.Vampire_BloodyKnife")
    );

    if (SoundObj.Succeeded())
    {
        FireSound = SoundObj.Object;
    }

}

void UDESkill_SanguineBats::InitBehaviors()
{
	Super::InitBehaviors();
    UDEBehavior_FireProjectile* FireAction = NewObject<UDEBehavior_FireProjectile>(this);

    // 2. 부품 설정 (이 스킬만의 특징 주입)
    FireAction->ProjectileClass = this->ProjectileClass; // "블러디 나이프를 던져라"
    FireAction->FireSound = this->FireSound;             // "이 소리를 내라"
    FireAction->BurstInterval = 0.0f;
    FireAction->RandomPositionRange = 0.0f;
    FireAction->FireConeAngle = 360.0f;



    // 3. 장착
    Behaviors.Add(FireAction);
}

void UDESkill_SanguineBats::ExecuteWithContext(FDESkillContext& Context)
{
    //UE_LOG(LogTemp, Warning, TEXT("Own Skill Logic of : %s before behaviors"), *GetName());
    Super::ExecuteWithContext(Context);
    //UE_LOG(LogTemp, Warning, TEXT("Own Skill Logic of : %s after behaviors"), *GetName());
}
