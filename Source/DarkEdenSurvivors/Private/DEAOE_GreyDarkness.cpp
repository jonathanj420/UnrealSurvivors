// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAOE_GreyDarkness.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"


ADEAOE_GreyDarkness::ADEAOE_GreyDarkness()
{
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>NiagaraRef(TEXT("/Game/DarkEden/Data/Niagara/NS_GreyDarkness.NS_GreyDarkness"));
	if (NiagaraRef.Succeeded())
	{
		NiagaraComp->SetAsset(NiagaraRef.Object);

	}
	// Darkness는 기본적으로 무한 지속 Aura
	bInfiniteDuration = true;
}

void ADEAOE_GreyDarkness::OnHitTarget(AActor* Target)
{
	Super::OnHitTarget(Target);

	// 2. 갈릭 전용 효과
	ApplyDarknessEffects(Target);
}


void ADEAOE_GreyDarkness::ApplyDarknessEffects(AActor* Target)
{
	//ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Target);
	//if (!Monster) return;

	///* ===================== Knockback 누적 ===================== */

	//float& CurrentKB = AccumulatedKnockback.FindOrAdd(Target);
	//CurrentKB = FMath::Min(CurrentKB + KnockbackPerHit, MaxAdditionalKnockback);

	//Monster->AddKnockbackBonus(CurrentKB);

	///* ===================== Freeze 저항 감소 ===================== */

	//float& FreezeResist = CurrentFreezeResist.FindOrAdd(Target);
	//if (FreezeResist <= 0.f)
	//{
	//	// 초기값 설정 (몬스터 기본 저항)
	//	FreezeResist = Monster->GetFreezeResistance();
	//}

	//FreezeResist = FMath::Max(FreezeResist - FreezeResistReduction, 0.f);
	//Monster->SetFreezeResistance(FreezeResist);
	//UE_LOG(LogTemp, Warning, TEXT("%s Hit By %s"), *GetNameSafe(Target), *GetNameSafe(this));
}