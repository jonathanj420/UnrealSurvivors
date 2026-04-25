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
    if (Radius > 0.f)
    {
        SetSize(Radius);
    }
    /*float NewSize = Context.GetValue(TEXT("Size"), -1.f);
    if (NewSize > 0.f)
    {
        SetActorScale3D(FVector(NewSize / 30.0f));
    }*/

    ResetState();
}

void ADESkillActorBase::BeginPlay()
{
    Super::BeginPlay();

    // ★ 게임 시작할 때 딱 1번만 모든 나이아가라를 뒤져서 명부에 적어둔다!
    GetComponents<UNiagaraComponent>(CachedNiagaraComps);
}

void ADESkillActorBase::ResetState()
{


    for (UNiagaraComponent* NComp : CachedNiagaraComps)
    {
        if (NComp)
        {
            NComp->SetVisibility(true);

            // ★ [핵심] 파티클이 꺼져 있을 때만 켭니다.
            // (영구 지속 오라가 스탯 갱신을 위해 ResetState를 탈 때는 깜빡이지 않고 부드럽게 유지됨!)
            if (!NComp->IsActive())
            {
                NComp->Activate(true);
            }
        }
    }

    //if (NiagaraComponent)
    //{
    //    NiagaraComponent->DeactivateImmediate(); // 트레일 히스토리 즉시 끊기
    //    NiagaraComponent->ResetSystem();         // 파티클 완전 초기화
    //    NiagaraComponent->SetVisibility(true);
    //    NiagaraComponent->Activate(true);
    //}

    if (LifeTime > 0.f)
    {
        GetWorldTimerManager().SetTimer(
            LifeTimeTimerHandle, this,
            &ADESkillActorBase::OnLifeTimeExpired,
            LifeTime, false);
    }

    SetActorHiddenInGame(false);
    SetActorTickEnabled(true);



    if (CollisionComponent)
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void ADESkillActorBase::ReturnToPool()
{
    if (CollisionComponent)
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorHiddenInGame(true);
    SetActorTickEnabled(false);
    GetWorldTimerManager().ClearTimer(LifeTimeTimerHandle);

    for (UNiagaraComponent* NComp : CachedNiagaraComps)
    {
        if (NComp)
        {
            NComp->DeactivateImmediate();
            NComp->SetVisibility(false);
        }
    }

    /*if (NiagaraComponent)
    {
        NiagaraComponent->Deactivate();
        NiagaraComponent->SetVisibility(false);
    }*/

    

    

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
    
    SetActorScale3D(FVector(NewSize / Radius));
    Radius = NewSize;

}