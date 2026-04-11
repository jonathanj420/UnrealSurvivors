// Fill out your copyright notice in the Description page of Project Settings.


#include "DESimpleSummonBase.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DEPoolSubsystem.h"
#include "DEMonsterBase.h"
#include "DEGameplayLibrary.h"
#include "DECombatEffect.h"
#include "DEAutoSkillBase.h"
#include "TimerManager.h"
// Sets default values
ADESimpleSummonBase::ADESimpleSummonBase()
{
	PrimaryActorTick.bCanEverTick = true; // 소환수 이동(추적) 로직을 위해 켜둠

	// 1. [충돌체 생성] (소환수의 중심점 및 겹침 판정용)
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = CollisionComponent;
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 기본은 Overlap으로

	// 2. [메시 생성]
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCastShadow(false);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);

	// 3. [나이아가라 생성]
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent);
	NiagaraComponent->SetAutoActivate(false); // ResetState에서 켬
	NiagaraComponent->SetRelativeLocation(FVector::ZeroVector);
}

void ADESimpleSummonBase::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void ADESimpleSummonBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADESimpleSummonBase::InitializeFromContext(const FDESkillContext& Context)
{
	
	if (APawn* InstigatorPawn = Cast<APawn>(Context.Instigator))
	{
		SetInstigator(InstigatorPawn);
	}

	CachedContext = Context;
	Snapshot = Context.FinalSnapshot;
	Damage = Context.Damage;
	CritChance = Context.CritChance;
	CritDamageMultiplier = Context.CritDamageMultiplier;
	KnockbackForce = Context.KnockbackForce;
	LifeTime = Context.Duration;
	EffectRadius = Context.Radius;
	UE_LOG(LogTemp, Warning, TEXT("WTF IS GOING ON : DMG: %f, Rad: %f"), Context.Damage, Context.Radius);
	// 크기 조절 (데이터 테이블에서 Size 값을 받아왔다면 적용)
	float NewSize = Context.GetValue(TEXT("Size"), -1.f);
	if (NewSize > 0.f)
	{
		SetActorScale3D(FVector(NewSize / 30.0f)); // 기준 반경(30) 대비 스케일링
	}

	ResetState();
}

void ADESimpleSummonBase::ResetState()
{
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);

	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	if (NiagaraComponent)
	{
		NiagaraComponent->SetVisibility(true);
		NiagaraComponent->ReinitializeSystem();

		NiagaraComponent->Activate(true);
	}

	// 수명 타이머 작동
	GetWorldTimerManager().SetTimer(LifeTimeTimerHandle, this, &ADESimpleSummonBase::OnLifeTimeExpired, LifeTime, false);
}

void ADESimpleSummonBase::ReturnToPool()
{
	GetWorldTimerManager().ClearTimer(LifeTimeTimerHandle);

	if (NiagaraComponent)
	{
		//NiagaraComponent->Deactivate();
		NiagaraComponent->DeactivateImmediate();
		NiagaraComponent->SetVisibility(false);
	}

	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);

	if (UWorld* World = GetWorld())
	{
		if (auto* Pool = World->GetSubsystem<UDEPoolSubsystem>())
		{
			Pool->ReturnActorToPool(this);
		}
	}
}

void ADESimpleSummonBase::OnLifeTimeExpired()
{
	ReturnToPool();
}

// 작성하신 완벽한 데미지 로직 그대로 복붙!
bool ADESimpleSummonBase::TryDealDamage(AActor* Victim)
{
	if (!Victim) return false;

	// 소환수 기준 넉백 방향
	FVector KBDir = (Victim->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();

	FDEDamageRequest Req;
	Req.Instigator = GetInstigator();
	Req.DamageCauser = this;
	Req.SourceObject = CachedContext.SourceSkill;
	Req.Victim = Victim;
	Req.BaseDamage = Damage;
	Req.CritChance = CritChance;
	Req.CritDamageMultiplier = CritDamageMultiplier;

	FDEDamageResult Res = UDEGameplayLibrary::ApplyCombatDamage(Req);

	if (Res.FinalDamage <= 0.0f)
	{
		return false;
	}

	if (Res.FinalDamage > 0.0f && this->KnockbackForce > 0.0f)
	{
		if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Req.Victim))
			Monster->ApplyKnockback(KBDir, this->KnockbackForce);
	}

	FCombatEventData EventData;
	EventData.Instigator = GetInstigator();
	EventData.Target = Victim;
	EventData.DamageAmount = Res.FinalDamage;

	for (UDECombatEffect* Effect : LocalEffects)
	{
		if (!Effect) continue;

		if (Effect->TriggerCondition == ECombatEventTrigger::OnHit)
		{
			Effect->ExecuteEffect(EventData);
		}
		else if (Effect->TriggerCondition == ECombatEventTrigger::OnKill && Res.bIsDead)
		{
			Effect->ExecuteEffect(EventData);
		}
	}

	return true;
}