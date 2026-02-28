#include "DESkillBase.h"
#include "DECombatComponent.h"
#include "GameFramework/Actor.h" // AActor 사용을 위해 필수

UDESkillBase::UDESkillBase()
{
	// 기본값 초기화
	CurrentLevel = 1;
	SkillOwner = nullptr;
}

UWorld* UDESkillBase::GetWorld() const
{
	// [중요] UObject는 기본적으로 World를 모릅니다.
	// 그래서 "내 주인(SkillOwner)이 속한 월드"를 반환하도록 연결해줘야 합니다.

	// 1. 주인이 있으면 주인의 월드 반환
	if (SkillOwner)
	{
		return SkillOwner->GetWorld();
	}

	// 2. 주인이 아직 없으면(CDO 등) Outer에서 찾아봄 (안전장치)
	if (const UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}

	return nullptr;
}

void UDESkillBase::InitSkill(AActor* InOwner)
{
	SkillOwner = InOwner;
	if (SkillOwner)
	{
		CachedCombatComp = SkillOwner->FindComponentByClass<UDECombatComponent>();
	}
}