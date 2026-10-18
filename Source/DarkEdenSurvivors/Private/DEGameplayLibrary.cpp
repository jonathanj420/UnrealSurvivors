// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameplayLibrary.h"
#include "DEHealthComponent.h"
#include "DECombatComponent.h"
#include "DEGameMode_Stage.h"
#include "DEStatusEffectComponent.h"
#include "DEMonsterBase.h"
#include "Engine/OverlapResult.h"
#include "DECombatTypes.h"

FDEDamageResult UDEGameplayLibrary::ApplyCombatDamage(
	const FDEDamageRequest& Request)
{
	if (Request.SourceObject == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ApplyCombatDamage called, SourceObject null"));
	}

	FDEDamageResult Result;

	if (!Request.Instigator || !Request.Victim) return Result;

	UDEHealthComponent* TargetHealth = Request.Victim->FindComponentByClass<UDEHealthComponent>();
	if (!TargetHealth) return Result;

	//modifiers and should be decoupled sooner
	FDEDamageRequest ModifiableReq = Request;

	if (UDECombatComponent* CombatComp = Request.Instigator->FindComponentByClass<UDECombatComponent>())
	{
		FCombatEventData PreHitData;
		PreHitData.Instigator = Request.Instigator;
		PreHitData.Target = Request.Victim;
		PreHitData.DamageMultiplier = 1.0f;

		//player's OnPreHit
		CombatComp->BroadcastCombatEvent(ECombatEventTrigger::OnPreHit, PreHitData);

		//ex) Dam Mul Modifier to Low HPs
		ModifiableReq.BaseDamage *= PreHitData.DamageMultiplier;

	}

	////Dam Modifier by Status Effects like Venom or Corrosion... but still should be decoupled
	if (UDEStatusEffectComponent* VictimStatusComp = Request.Victim->FindComponentByClass<UDEStatusEffectComponent>())
	{
		
		VictimStatusComp->ProcessIncomingDamageModifiers(ModifiableReq);
	}

	//process with final Req
	Result = TargetHealth->ProcessDamage(ModifiableReq);
	//UE_LOG(LogTemp, Warning, TEXT("Processed Final Damage of Result : %f"), Result.FinalDamage);

	Result.Victim = ModifiableReq.Victim;
	Result.SourceObject = ModifiableReq.SourceObject;
	Result.DamageTags = ModifiableReq.DamageTags;
	Result.bCanTriggerOnHit = ModifiableReq.bCanTriggerOnHit;

	//try handle OnHit or other effects of instigator 
	if (UDECombatComponent* CombatComp = Request.Instigator->FindComponentByClass<UDECombatComponent>())
	{
		CombatComp->HandleDamageDealt(Result);
	}

	if (TargetHealth)
	{
		//ensure isDead
		Result.bIsDead = TargetHealth->IsDead();
	}

	return Result;

}

void UDEGameplayLibrary::ApplyAoEDamage(UWorld* World, const FVector& Origin, float Radius, const FDEDamageRequest& BaseRequest, const TArray<AActor*>& IgnoredActors)
{
	if (!World) return;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(IgnoredActors);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel5);

	//OverlapMultiByChannel -> OverlapMultiByObjectType
	bool bHit = World->OverlapMultiByObjectType(
		OverlapResults,
		Origin,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(Radius),
		QueryParams
	);
	//DrawDebugSphere(World, Origin, Radius, 16, FColor::Green, false, 1.0f, 0, 1.0f);
	if (!bHit) return;

	for (const FOverlapResult& Hit : OverlapResults)
	{
		AActor* Victim = Hit.GetActor();
		if (!Victim) continue;

		FDEDamageRequest TargetReq = BaseRequest;
		TargetReq.Victim = Victim;

		ApplyCombatDamage(TargetReq);
	}

}

//get the nearest target by radius
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

//get random targets by radius
TArray<AActor*> UDEGameplayLibrary::GetRandomTargets(AActor* Instigator, float Radius, int32 Count)
{
	TArray<AActor*> Result;
	if (!Instigator || Count <= 0) return Result;

	UWorld* World = Instigator->GetWorld();
	if (!World) return Result;

	TArray<AActor*> Candidates;

	if (FMath::IsNearlyEqual(Radius, -1.0f))
	{
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

	if (Candidates.Num() == 0) return Result;

	// Fisher-Yates
	for (int32 i = Candidates.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		Candidates.Swap(i, j);
	}

	int32 FinalCount = FMath::Min(Count, Candidates.Num());
	for (int32 i = 0; i < FinalCount; i++)
	{
		Result.Add(Candidates[i]);
	}

	return Result;
}

