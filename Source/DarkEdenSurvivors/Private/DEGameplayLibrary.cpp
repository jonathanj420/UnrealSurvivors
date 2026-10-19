// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameplayLibrary.h"
#include "DEMonsterQuerySubsystem.h"
#include "Engine/World.h"
#include "DEHealthComponent.h"
#include "DECombatComponent.h"
#include "DEGameMode_Stage.h"
#include "DEStatusEffectComponent.h"
#include "DEMonsterBase.h"
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

	const UDEMonsterQuerySubsystem* Query = World->GetSubsystem<UDEMonsterQuerySubsystem>();
	if (!Query) return;

	TArray<ADEMonsterBase*> Victims;
	Query->QueryCircle(Origin, Radius, Victims);

	for (ADEMonsterBase* Victim : Victims)
	{
		if (IgnoredActors.Contains(Victim)) continue;

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

	const UDEMonsterQuerySubsystem* Query = World->GetSubsystem<UDEMonsterQuerySubsystem>();
	if (!Query) return nullptr;

	// Radius가 음수(-1)면 거리 제한 없이 맵 전체에서 찾는다
	return Query->QueryNearest(Instigator->GetActorLocation(), Radius);
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
	else if (const UDEMonsterQuerySubsystem* Query = World->GetSubsystem<UDEMonsterQuerySubsystem>())
	{
		TArray<ADEMonsterBase*> InRange;
		Query->QueryCircle(Instigator->GetActorLocation(), Radius, InRange);
		Candidates.Append(InRange);
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

