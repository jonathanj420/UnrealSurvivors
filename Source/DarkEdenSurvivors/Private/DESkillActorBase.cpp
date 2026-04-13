// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillActorBase.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "DEPoolSubsystem.h"
#include "DEGameplayLibrary.h"
#include "DEMonsterBase.h"
#include "DECombatEffect.h"
#include "DEAutoSkillBase.h"

// Sets default values
ADESkillActorBase::ADESkillActorBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. 공통 충돌체
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = CollisionComponent;
    CollisionComponent->InitSphereRadius(30.0f);

    // 2. 공통 메시
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetCastShadow(false);
    Mesh->SetSimulatePhysics(false);
    Mesh->SetEnableGravity(false);

    // 3. 공통 이펙트
    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
    NiagaraComponent->SetupAttachment(RootComponent);
    NiagaraComponent->SetAutoActivate(false);

}


void ADESkillActorBase::InitializeFromContext(const FDESkillContext& Context)
{
    if (APawn* InstigatorPawn = Cast<APawn>(Context.Instigator))
    {
        SetInstigator(InstigatorPawn);
    }
    if (Context.Instigator)
    {
        SetOwner(Context.Instigator);
    }


    // 데이터 캐싱
    CachedContext = Context;
    Snapshot = Context.FinalSnapshot;
    Damage = Context.Damage;
    Radius = Context.Radius;
    if (bCanCrit)
    {
        CritChance = Context.CritChance;
        CritDamageMultiplier = Context.CritDamageMultiplier;
    }
    KnockbackForce = Context.KnockbackForce;
    LifeTime = Context.Duration;

    // 크기 조절
    float NewSize = Context.GetValue(TEXT("Size"), -1.f);
    if (NewSize > 0.f)
    {
        SetActorScale3D(FVector(NewSize / 30.0f));
    }

    ResetState();
}

void ADESkillActorBase::ResetState()
{
    SetActorHiddenInGame(false);
    SetActorTickEnabled(true);

    if (CollisionComponent)
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    if (NiagaraComponent)
    {
        NiagaraComponent->SetVisibility(true);
        NiagaraComponent->ReinitializeSystem();
        NiagaraComponent->Activate(true);
    }

    GetWorldTimerManager().SetTimer(LifeTimeTimerHandle, this, &ADESkillActorBase::OnLifeTimeExpired, LifeTime, false);
}

void ADESkillActorBase::ReturnToPool()
{
    GetWorldTimerManager().ClearTimer(LifeTimeTimerHandle);

    if (NiagaraComponent)
    {
        NiagaraComponent->Deactivate();
        NiagaraComponent->SetVisibility(false);
    }

    if (CollisionComponent)
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

void ADESkillActorBase::OnLifeTimeExpired()
{
    ReturnToPool();
}

bool ADESkillActorBase::TryDealDamage(AActor* Victim)
{
    if (!Victim) return false;

    FVector KBDir = GetActorForwardVector().GetSafeNormal2D();

    FDEDamageRequest Req;
    Req.Instigator = GetInstigator();
    Req.DamageCauser = this;
    Req.SourceObject = CachedContext.SourceSkill;
    Req.Victim = Victim;
    Req.BaseDamage = Damage;
    Req.CritChance = CritChance;
    Req.CritDamageMultiplier = CritDamageMultiplier;

    FDEDamageResult Res = UDEGameplayLibrary::ApplyCombatDamage(Req);

    if (Res.FinalDamage <= 0.0f) return false;

    if (KnockbackForce > 0.0f)
    {
        if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Req.Victim))
            Monster->ApplyKnockback(KBDir, KnockbackForce);
    }

    FCombatEventData EventData;
    EventData.Instigator = GetInstigator();
    EventData.Target = Victim;
    EventData.DamageAmount = Res.FinalDamage;
    EventData.SourceContext = &CachedContext;

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

void ADESkillActorBase::SetSize(float NewSize)
{
    // 1. 데이터 갱신
    Size = NewSize;

    // 2. 스케일 계산 (기본 반지름을 30.0f라고 가정했을 때 비율 계산)
    // 만약 기본 반지름이 30인데 33이 들어오면 -> 1.1배 커짐 (+10%)
    const float DefaultRadius = 30.0f;
    float NewScale = NewSize / DefaultRadius;

    // 3. 액터 전체 크기 변경 (충돌체 + 나이아가라 모두 커짐)
    // 이렇게 하면 따로 나이아가라 파라미터를 건드릴 필요 없이 다 같이 커집니다.
    SetActorScale3D(FVector(NewScale));

    // (참고) 만약 스케일 말고 충돌체만 정밀하게 조절하고 싶다면 아래 코드를 씁니다.
    // 하지만 뱀서류는 그냥 SetActorScale3D 쓰는 게 성능도 좋고 관리도 편합니다.
    /*
    if (CollisionComponent)
    {
        CollisionComponent->SetSphereRadius(Size);
    }
    */
}