// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"

// 진영. 팀 ID는 IGenericTeamAgentInterface로 노출한다 (AIController, AIPerception이 그대로 쓴다).
// 몬스터는 항상 Monster, 캐릭터는 빙의한 컨트롤러로 정한다 (플레이어 컨트롤러면 Player, AI 마스터면 Monster)
namespace DETeam
{
	inline const FGenericTeamId Player(0);
	inline const FGenericTeamId Monster(1);

	// 둘 다 팀이 있고 팀이 다르면 적대. 팀이 없는 쪽(NoTeam)은 누구와도 적대가 아니다
	inline bool AreHostile(FGenericTeamId A, FGenericTeamId B)
	{
		return A != FGenericTeamId::NoTeam && B != FGenericTeamId::NoTeam && A != B;
	}

	// 둘 다 팀이 있고 같은 팀이면 아군
	inline bool AreFriendly(FGenericTeamId A, FGenericTeamId B)
	{
		return A != FGenericTeamId::NoTeam && A == B;
	}

	inline FGenericTeamId GetTeam(const AActor* Actor)
	{
		return FGenericTeamId::GetTeamIdentifier(Actor);
	}
}
