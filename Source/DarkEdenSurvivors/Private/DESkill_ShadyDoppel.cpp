// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_ShadyDoppel.h"
#include "DEBehavior_FireProjectile.h"
#include "DEGameplayLibrary.h"

UDESkill_ShadyDoppel::UDESkill_ShadyDoppel()
{
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

void UDESkill_ShadyDoppel::InitBehaviors()
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
    FireAction->FireConeAngle = 180.0f;
    FireAction->RandomPositionRange = 40.0f;


    // 3. 장착
    Behaviors.Add(FireAction);
}

void UDESkill_ShadyDoppel::ExecuteWithContext(FDESkillContext& Context)
{

    Context.Targets.Reset();
    Context.Targets.Append(UDEGameplayLibrary::GetRandomTargets(SkillOwner, -1.0f, 1));

    // 부모 함수 호출 (PlayFX -> 부채꼴 타겟팅 -> 데미지 파이프라인 실행)
    Super::ExecuteWithContext(Context);

}