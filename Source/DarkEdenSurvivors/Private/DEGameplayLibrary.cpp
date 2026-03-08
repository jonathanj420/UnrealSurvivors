// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameplayLibrary.h"
#include "DEHealthComponent.h"
#include "DECombatComponent.h"
#include "DEGameMode_Stage.h"
#include "DEMonsterBase.h"
#include "Engine/OverlapResult.h"

FDEDamageResult UDEGameplayLibrary::ApplyCombatDamage(
	const FDEDamageRequest& Request,
	const FCombatSnapshot& Snapshot,
	FVector KnockbackDir,
	float KnockbackForce)
{
	FDEDamageResult Result;

	// 1. 유효성 검사 (때린 놈, 맞은 놈 확인)
	if (!Request.Instigator || !Request.Victim)
	{

		if (!Request.Instigator) UE_LOG(LogTemp, Error, TEXT("ApplyCombatDamage Error: Instigator is NULL!"));
		if (!Request.Victim) UE_LOG(LogTemp, Error, TEXT("ApplyCombatDamage Error: Victim is NULL!"));
		return Result;
	}
	
	UDEHealthComponent* TargetHealth = Request.Victim->FindComponentByClass<UDEHealthComponent>();
	if (!TargetHealth)
	{
		UE_LOG(LogTemp, Error, TEXT("ApplyCombatDamage Error: No Victim HealthComponent"));
		return Result;
	}

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

AActor* UDEGameplayLibrary::GetNearestTarget(AActor* Instigator, float Radius)
{
	if (!Instigator) return nullptr;

	UWorld* World = Instigator->GetWorld();
	if (!World) return nullptr;

	FVector MyLoc = Instigator->GetActorLocation();
	AActor* Nearest = nullptr;
	float MinDistSq = FLT_MAX;

	if (FMath::IsNearlyEqual(Radius, -1.0f))
	{
		auto* GM = Cast<ADEGameMode_Stage>(World->GetAuthGameMode());
		if (GM)
		{
			const TArray<ADEMonsterBase*>& ActiveMonsters = GM->GetActiveMonsters();
			for (ADEMonsterBase* Monster : ActiveMonsters)
			{
				if (!Monster->IsAlive()) continue;
				float DistSq = FVector::DistSquared(MyLoc, Monster->GetActorLocation());
				if (DistSq < MinDistSq)
				{
					MinDistSq = DistSq;
					Nearest = Monster;
				}
			}

		}

		return Nearest;
	}
	

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Monster);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Instigator);

	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByObjectType(
		Overlaps, MyLoc, FQuat::Identity,
		ObjectParams,
		FCollisionShape::MakeSphere(Radius),
		QueryParams
	);

	

	for (const FOverlapResult& Res : Overlaps)
	{
		AActor* Actor = Res.GetActor();
		if (!Actor || !Actor->IsA(ADEMonsterBase::StaticClass())) continue;

		float DistSq = FVector::DistSquared(MyLoc, Actor->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			Nearest = Actor;
		}
	}

	return Nearest;
}

TArray<AActor*> UDEGameplayLibrary::GetRandomTargets(AActor* Instigator, float Radius, int32 Count)
{
	TArray<AActor*> Result;
	if (!Instigator || Count <= 0) return Result;

	UWorld* World = Instigator->GetWorld();
	if (!World) return Result;

	TArray<AActor*> Candidates;

	// --- 1. 후보군(Candidates) 채우기 ---
	if (FMath::IsNearlyEqual(Radius, -1.0f))
	{
		// [전체 범위 모드] GameMode에서 바로 가져오기
		auto* GM = Cast<ADEGameMode_Stage>(World->GetAuthGameMode());
		if (GM)
		{
			for (ADEMonsterBase* Monster : GM->GetActiveMonsters())
			{
				if (Monster) Candidates.Add(Monster);
			}
		}
	}
	else
	{
		// [반경 탐색 모드] OverlapMulti 사용
		FVector MyLoc = Instigator->GetActorLocation();
		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_Monster);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Instigator);

		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(
			Overlaps, MyLoc, FQuat::Identity,
			ObjectParams,
			FCollisionShape::MakeSphere(Radius),
			QueryParams
		);

		for (const FOverlapResult& Res : Overlaps)
		{
			AActor* Actor = Res.GetActor();
			if (Actor && Actor->IsA(ADEMonsterBase::StaticClass()))
			{
				Candidates.Add(Actor);
			}
		}
	}

	// --- 2. 공통 로직: 셔플 및 추출 ---
	if (Candidates.Num() == 0) return Result;

	// Fisher-Yates 셔플
	for (int32 i = Candidates.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		Candidates.Swap(i, j);
	}

	// 앞에서 Count개 뽑기 (셔플 덕분에 랜덤하게 뽑힘)
	int32 FinalCount = FMath::Min(Count, Candidates.Num());
	for (int32 i = 0; i < FinalCount; i++)
	{
		Result.Add(Candidates[i]);
	}

	return Result;
}

