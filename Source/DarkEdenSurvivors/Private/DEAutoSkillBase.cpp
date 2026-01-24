// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAutoSkillBase.h"
#include "DESkillBehavior.h"

void UDEAutoSkillBase::Activate()
{
	if (!SkillOwner) return;

	FDESkillContext Context;
	BuildContext(Context);

	for (UDESkillBehavior* Behavior : Behaviors)
	{
		if (Behavior) Behavior->Execute(Context);
	}
}

void UDEAutoSkillBase::SetSkillData(const FDESkillData* NewData)
{
	SkillData = NewData;
	if (SkillData) CurrentLevel = SkillData->Level;
	UE_LOG(LogTemp, Warning, TEXT("[Skill] %s : level : %d, "), *NewData->SkillName,NewData->Level);
}

void UDEAutoSkillBase::InitBehaviors()
{
	Behaviors.Empty();
}

void UDEAutoSkillBase::BuildContext(FDESkillContext& OutContext)
{
	OutContext.Instigator = SkillOwner;

	if (SkillData)
	{
		// 1. 메인 변수 복사
		OutContext.Damage = SkillData->Damage;
		OutContext.ProjectileCount = SkillData->ProjectileCount;
		OutContext.Penetration = SkillData->Penetration;
		OutContext.Speed = SkillData->ProjectileSpeed;
		OutContext.KnockbackForce = SkillData->KnockbackForce;
		OutContext.Radius = SkillData->Radius;

		// 2. 맵 데이터(옵션) 통째로 복사
		OutContext.CustomValues = SkillData->OptionValues;
	}
}