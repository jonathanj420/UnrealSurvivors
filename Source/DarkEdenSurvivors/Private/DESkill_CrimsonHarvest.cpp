// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_CrimsonHarvest.h"
#include "UObject/ConstructorHelpers.h"
#include "DEBehavior_FireProjectile.h"

UDESkill_CrimsonHarvest::UDESkill_CrimsonHarvest()
{
    // 1. 경로 설정 (블루프린트 에셋 우클릭 -> '레퍼런스 복사' 후 _C를 꼭 붙여주세요)
    static ConstructorHelpers::FClassFinder<AActor> ProjectileBP(TEXT("/Game/DarkEden/Blueprint/SkillProjectiles/BP_DEProjectile_CrimsonHarvest.BP_DEProjectile_CrimsonHarvest_C"));

    // 2. 에셋을 성공적으로 찾았는지 확인 후 변수에 할당
    if (ProjectileBP.Succeeded())
    {
        // ProjectileClass는 보통 헤더에 TSubclassOf<AActor>로 선언합니다.
        ProjectileClass = ProjectileBP.Class;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Bat_wing.Bat_wing")
    );

    if (SoundObj.Succeeded())
    {
        FireSound = SoundObj.Object;
    }

}

void UDESkill_CrimsonHarvest::InitBehaviors()
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