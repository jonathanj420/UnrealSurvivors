// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_Darkness.h"
#include "DEBehavior_SpawnAOE.h"
// AOE
#include "DEAOE_Darkness.h"

// Context
#include "DESkillContext.h"
UDESkill_Darkness::UDESkill_Darkness()
{
	AOEClass = ADEAOE_Darkness::StaticClass();

}

void UDESkill_Darkness::InitBehaviors()
{
    UDEBehavior_SpawnAOE* SpawnDarkness = NewObject<UDEBehavior_SpawnAOE>(this);

    if (SpawnDarkness)
    {
        // 2. Context가 아니라, 비헤이비어 본체에 직접 세팅을 주입!
        SpawnDarkness->AOEClass = AOEClass;               // 블루프린트에서 할당한 마늘 오라 클래스
        SpawnDarkness->AOEKey = TEXT("Darkness");         // 중복 생성 방지용 영구 키

        // ★ 3. 우리가 만든 범용 타겟팅 시스템 적용!
        SpawnDarkness->SpawnTarget = EAOESpawnTarget::Instigator; // 타겟: "내 위치에 스폰해라!"
        SpawnDarkness->bAttachToTarget = true;                    // 옵션: "스폰 즉시 내 몸에 찰싹 붙여라!"

        // 4. 파이프라인에 장착
        Behaviors.Add(SpawnDarkness);
    }
}
