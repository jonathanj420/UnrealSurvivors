// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_BloodyKnife.h"
#include "UObject/ConstructorHelpers.h"
#include "DEProjectile_BloodyKnife.h"
#include "Kismet/GameplayStatics.h"
//#include "DEPoolSubsystem.h"
#include "DEBehavior_FireProjectile.h"



UDESkill_BloodyKnife::UDESkill_BloodyKnife()
{
    ProjectileClass = ADEProjectile_BloodyKnife::StaticClass();
    // 1. 경로 설정 (블루프린트 에셋 우클릭 -> '레퍼런스 복사' 후 _C를 꼭 붙여주세요)
    static ConstructorHelpers::FClassFinder<AActor> ProjectileBP(TEXT("/Game/DarkEden/Blueprint/SkillProjectiles/BP_DEProjectile_ShadyDoppel1.BP_DEProjectile_ShadyDoppel1_C"));

    // 2. 에셋을 성공적으로 찾았는지 확인 후 변수에 할당
    if (ProjectileBP.Succeeded())
    {
        // SummonClass 는 보통 헤더에 TSubclassOf<AActor>로 선언합니다.
        ProjectileClass = ProjectileBP.Class;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Misc/Ou_Hit.Ou_Hit")
    );

    if (SoundObj.Succeeded())
    {
        FireSound = SoundObj.Object;
    }
}

void UDESkill_BloodyKnife::InitBehaviors()
{
    Super::InitBehaviors();
    // 나중에 여기에 각 스킬에 맞는 Behavior 추가하면 됨
    // 
    // 1. 행동(부품) 생성
    UDEBehavior_FireProjectile* FireAction = NewObject<UDEBehavior_FireProjectile>(this);

    // 2. 부품 설정 (이 스킬만의 특징 주입)
    FireAction->ProjectileClass = this->ProjectileClass; // "블러디 나이프를 던져라"
    FireAction->FireSound = this->FireSound;             // "이 소리를 내라"
    FireAction->BurstInterval = 0.1f;
    FireAction->RandomPositionRange = 40.0f;


    // 3. 장착
    Behaviors.Add(FireAction);
}

