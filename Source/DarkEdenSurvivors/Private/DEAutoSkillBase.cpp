// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAutoSkillBase.h"

UDEAutoSkillBase::UDEAutoSkillBase()
{
}

FSkillSpec UDEAutoSkillBase::GetFinalSpec(int32 CurrentLevel) const
{
    FSkillSpec Spec;

    if (!RowData) return Spec;

    Spec.Level = CurrentLevel;
    Spec.Damage = RowData->Damage;
    Spec.ProjectileCount = RowData->ProjectileCount;
    Spec.Penetration = RowData->Penetration;
    Spec.Cooldown = RowData->Cooldown;

    // 예시: 캐릭터 공격력 보정
    if (SkillOwner)
    {
        float CharDamageMultiplier = 1.f;
        // 캐릭터에 공격력 증가 옵션이 있다고 가정
        // CharDamageMultiplier = SkillOwner->GetDamageMultiplier();
        Spec.Damage *= CharDamageMultiplier;
    }

    // 나중에 스킬 강화 옵션 적용 가능
    // 예: 특정 스킬이면 ProjectileCount +1
    // if (RowData->SkillID == SPECIAL_SKILL_ID) Spec.ProjectileCount += 1;

    return Spec;
}

