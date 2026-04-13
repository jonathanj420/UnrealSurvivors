// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_DeadlyGround.h"
#include "UObject/ConstructorHelpers.h"
#include "DEBehavior_SpawnSummon.h"
#include "DEBehavior_SelectNearestTarget.h"

UDESkill_DeadlyGround::UDESkill_DeadlyGround()
{
    // 1. 경로 설정 (블루프린트 에셋 우클릭 -> '레퍼런스 복사' 후 _C를 꼭 붙여주세요)
    static ConstructorHelpers::FClassFinder<AActor> SummonBP(TEXT("/Game/DarkEden/Blueprint/SkillProjectiles/DESummon_DeadlyGround.DESummon_DeadlyGround_C"));

    // 2. 에셋을 성공적으로 찾았는지 확인 후 변수에 할당
    if (SummonBP.Succeeded())
    {
        // SummonClass 는 보통 헤더에 TSubclassOf<AActor>로 선언합니다.
        SummonClass = SummonBP.Class;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Vampire_Doom.Vampire_Doom")
    );

    if (SoundObj.Succeeded())
    {
        SummonSound = SoundObj.Object;
    }
    bCooldownAfterDuration = true;
}

void UDESkill_DeadlyGround::InitBehaviors()
{
    Super::InitBehaviors();

    // 1. [탐색 행동] 스폰할 위치(적) 찾기
  /*  UDEBehavior_SelectNearestTarget* Targeting = NewObject<UDEBehavior_SelectNearestTarget>(this);
    Targeting->DefaultSearchRadius = 1000.f;
    Targeting->EnemyTag = TEXT("Enemy");
    Behaviors.Add(Targeting);*/

    // 2. [소환 행동] 찾은 적 위치에 고어 글랜드 스폰!
    UDEBehavior_SpawnSummon* Spawner = NewObject<UDEBehavior_SpawnSummon>(this);
    Spawner->SummonClass = this->SummonClass; // 고어글랜드 클래스 할당
    Spawner->SpawnSound = this->SummonSound;
    Spawner->bUseBaseAmountOnly = false;
    Behaviors.Add(Spawner);
}
