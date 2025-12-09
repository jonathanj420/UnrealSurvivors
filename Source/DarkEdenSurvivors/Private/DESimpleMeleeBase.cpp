// Fill out your copyright notice in the Description page of Project Settings.


#include "DESimpleMeleeBase.h"
#include "DEMonsterBase.h"
#include "DEPoolSubsystem.h"


// Sets default values
ADESimpleMeleeBase::ADESimpleMeleeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
}

// Called when the game starts or when spawned
void ADESimpleMeleeBase::BeginPlay()
{
	Super::BeginPlay();
	if (NiagaraEffectComponent)
	{
		NiagaraEffectComponent->OnSystemFinished.AddDynamic(
			this, &ADESimpleMeleeBase::OnNiagaraFinished
		);
	}
}

// Called every frame
void ADESimpleMeleeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADESimpleMeleeBase::PerformSweepAttack()
{
	ResetState();
	if (!GetOwner()) return;

	// 공격 시작 위치
	FVector Start = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	// Sweep 범위
	TArray<FHitResult> HitResults;
	FCollisionShape SweepShape = FCollisionShape::MakeSphere(SweepRadius);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		Start + Forward * SweepRadius,
		FQuat::Identity,
		ECC_Pawn,
		SweepShape
	);

	for (auto& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);

			// 데미지 적용
			UGameplayStatics::ApplyDamage(HitActor, Damage, nullptr, this, nullptr);

			// 넉백 처리
			if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(HitActor))
			{
				FVector KnockDir = (HitActor->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
				Monster->ApplyKnockback(KnockDir, KnockbackForce);

			}
		}
	}

	if (NiagaraSystem)
	{
		float ScaleFactor = SweepRadius / BaseRadius; // BaseRadius는 이펙트 기본 크기
		NiagaraEffectComponent->SetWorldScale3D(FVector(ScaleFactor));
		NiagaraEffectComponent->SetAsset(NiagaraSystem);
		NiagaraEffectComponent->Activate(true);
	}
}

void ADESimpleMeleeBase::OnNiagaraFinished(UNiagaraComponent* PSystem)
{
	ReturnToPool();
}

void ADESimpleMeleeBase::ReturnToPool()
{
	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (auto* Pool = GI->GetSubsystem<UDEPoolSubsystem>())
			{
				Pool->ReturnActorToPool(this);
			}
		}
	}
}

void ADESimpleMeleeBase::ResetState()
{
	HitActors.Empty();
}