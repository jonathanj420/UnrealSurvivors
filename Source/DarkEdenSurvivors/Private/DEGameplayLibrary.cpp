// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameplayLibrary.h"
#include "DEHealthComponent.h"
#include "DECombatComponent.h"
#include "DEMonsterBase.h"


FDEDamageResult UDEGameplayLibrary::ApplyCombatDamage(
	const FDEDamageRequest& Request,
	const FCombatSnapshot& Snapshot,
	FVector KnockbackDir,
	float KnockbackForce)
{
	FDEDamageResult Result;

	if (!Request.Instigator) UE_LOG(LogTemp, Error, TEXT("ApplyCombatDamage Error: Instigator is NULL!"));
	if (!Request.Victim) UE_LOG(LogTemp, Error, TEXT("ApplyCombatDamage Error: Victim is NULL!"));

	// 1. 유효성 검사 (때린 놈, 맞은 놈 확인)
	if (!Request.Instigator || !Request.Victim) return Result;
	UDEHealthComponent* TargetHealth = Request.Victim->FindComponentByClass<UDEHealthComponent>();
	if (!TargetHealth) return Result;

	// 2. 피해자에게 데미지 처리 요청 (전달받은 Request 그대로 사용)
	Result = TargetHealth->ProcessDamage(Request);

	// 3. 가해자에게 후처리(피흡, 킬) 요청
	if (UDECombatComponent* CombatComp = Request.Instigator->FindComponentByClass<UDECombatComponent>())
	{
		CombatComp->HandleDamageDealt(Result, Snapshot);
	}

	// 4. 넉백 처리
	if (Result.FinalDamage > 0.0f && KnockbackForce > 0.0f)
	{
		if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Request.Victim))
		{
			Monster->ApplyKnockback(KnockbackDir, KnockbackForce);
		}
	}

	return Result;
}


//FDEDamageResult UDEGameplayLibrary::ApplyCombatDamage(
//	AActor* Instigator,
//	AActor* DamageCauser,
//	AActor* Target,
//	float BaseDamage,
//	const FCombatSnapshot& Snapshot,
//	FVector KnockbackDir,
//	float KnockbackForce)
//{
//	FDEDamageResult Result; // 빈 결과 생성
//
//	// 1. 유효성 검사 (때린 놈, 맞은 놈, 맞은 놈 체력통 확인)
//	if (!Instigator || !Target) return Result;
//
//	UDEHealthComponent* TargetHealth = Target->FindComponentByClass<UDEHealthComponent>();
//	if (!TargetHealth) return Result;
//
//	// 2. 데미지 신청서(Request) 작성
//	FDEDamageRequest Req;
//	Req.Instigator = Instigator;
//	Req.DamageCauser = DamageCauser;
//	Req.BaseDamage = BaseDamage;
//
//	// 스냅샷에서 전투 정보 가져오기
//	Req.CritChance = Snapshot.CritChance;
//	Req.CritDamageMultiplier = Snapshot.CritDamageMultiplier;
//	Req.LifeStealChance = Snapshot.LifeStealChance;
//	// (참고: LifeStealChance는 Request에 안 담고 아래 4번 단계에서 Snapshot을 직접 써도 됩니다. 
//	//  다만 Request에 담으면 HealthComponent가 로그 찍을 때 편합니다.)
//
//	// 3. 피해자에게 데미지 처리 요청 (실제 체력 차감)
//	// 피해자는 "나 맞았어" 처리만 하고, 결과(Result)를 리턴함
//	Result = TargetHealth->ProcessDamage(Req);
//
//	// 4. ★ [핵심] 가해자에게 "너 때렸어"라고 알려줌 (후처리)
//	// 가해자의 CombatComponent를 찾아서 피흡, 킬 보너스 등을 처리
//	if (Instigator)
//	{
//		if (UDECombatComponent* CombatComp = Instigator->FindComponentByClass<UDECombatComponent>())
//		{
//			CombatComp->HandleDamageDealt(Result, Snapshot);
//		}
//	}
//
//	// 5. 넉백 처리 (데미지가 조금이라도 박혔고, 넉백 힘이 있을 때만)
//	if (Result.FinalDamage > 0.0f && KnockbackForce > 0.0f)
//	{
//		// 몬스터라면 밀어내기
//		if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Target))
//		{
//			Monster->ApplyKnockback(KnockbackDir, KnockbackForce);
//		}
//	}
//
//	return Result;
//}
