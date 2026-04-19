// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_GreyDarkness.h"
#include "DEBehavior_SpawnAOE.h"
#include "DEAOE_GreyDarkness.h"
UDESkill_GreyDarkness::UDESkill_GreyDarkness()
{
	AOEClass = ADEAOE_GreyDarkness::StaticClass();
}

void UDESkill_GreyDarkness::InitBehaviors()
{
    UDEBehavior_SpawnAOE* SpawnDarkness = NewObject<UDEBehavior_SpawnAOE>(this);

    if (SpawnDarkness)
    {
        // 2. Context가 아니라, 비헤이비어 본체에 직접 세팅을 주입!
        SpawnDarkness->AOEClass = AOEClass;               // 블루프린트에서 할당한 마늘 오라 클래스
        SpawnDarkness->AOEKey = TEXT("GreyDarkness");         // 중복 생성 방지용 영구 키

        // ★ 3. 우리가 만든 범용 타겟팅 시스템 적용!
        SpawnDarkness->SpawnTarget = EAOESpawnTarget::Instigator; // 타겟: "내 위치에 스폰해라!"
        SpawnDarkness->bAttachToTarget = true;                    // 옵션: "스폰 즉시 내 몸에 찰싹 붙여라!"

        // 4. 파이프라인에 장착
        Behaviors.Add(SpawnDarkness);
    }
}

void UDESkill_GreyDarkness::OnTargetKilled(const FDEDamageResult& Result)
{
    if (AccumulatedDamage < MaxBonusDamage)
    {
        AccumulatedDamage = FMath::Min(AccumulatedDamage + BonusDamagePerKill, MaxBonusDamage);
        AccumulatedRadius = FMath::Min(AccumulatedRadius + BonusRadiusPerKill, MaxBonusRadius);
        RefreshContext();
        UE_LOG(LogTemp, Warning, TEXT("Grey Darkness Damage Increased"));
    }

}

void UDESkill_GreyDarkness::BuildContext(FDESkillContext& OutContext)
{
    // 1. 부모(DEAutoSkillBase)의 기본 로직(플레이어 스탯 배율 연동 등) 실행
    Super::BuildContext(OutContext);

    // 2. 내가 죽여서 모아둔 핏빛 축제(보너스 스택)를 최종 결과에 더해준다!
    OutContext.Damage += AccumulatedDamage;
    OutContext.Radius += AccumulatedRadius;

}
