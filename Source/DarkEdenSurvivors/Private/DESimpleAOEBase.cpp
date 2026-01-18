// Fill out your copyright notice in the Description page of Project Settings.


#include "DESimpleAOEBase.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DEPoolSubsystem.h"

ADESimpleAOEBase::ADESimpleAOEBase()
{
    PrimaryActorTick.bCanEverTick = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
    NiagaraComp->SetupAttachment(RootComponent);
}

void ADESimpleAOEBase::InitializeAOE(
    float InDamage,
    float InRadius,
    float InTickInterval,
    int32 InMaxTickCount,
    float InLifeTime,
    AActor* InOwner
)
{
    Damage = InDamage;
    TickInterval = InTickInterval;
    MaxTickCount = InMaxTickCount;
    LifeTime = InLifeTime;
    OwnerActor = InOwner;

    Collision->SetSphereRadius(InRadius);

    CurrentTickCount = 0;
}

void ADESimpleAOEBase::BeginPlay()
{
    Super::BeginPlay();

    if (TickInterval <= 0.f)
    {
        // 즉발 AOE
        ApplyDamageOnce();
        SetLifeSpan(LifeTime);
    }
    else
    {
        // 지속 AOE
        GetWorldTimerManager().SetTimer(
            TickTimerHandle,
            this,
            &ADESimpleAOEBase::TickDamage,
            TickInterval,
            true
        );
    }
}

void ADESimpleAOEBase::ApplyDamageOnce()
{
    TArray<AActor*> Overlapped;
    Collision->GetOverlappingActors(Overlapped);

    for (AActor* Target : Overlapped)
    {
        if (!Target || Target == OwnerActor.Get()) continue;

        UGameplayStatics::ApplyDamage(
            Target,
            Damage,
            nullptr,
            OwnerActor.Get(),
            nullptr
        );
    }
}

void ADESimpleAOEBase::TickDamage()
{
    ApplyDamageOnce();
    CurrentTickCount++;

    if (MaxTickCount > 0 && CurrentTickCount >= MaxTickCount)
    {
        FinishAOE();
    }
}

void ADESimpleAOEBase::FinishAOE()
{
    GetWorldTimerManager().ClearTimer(TickTimerHandle);
    ReturnToPool(); // 풀링 쓰면 여기서 ReturnToPool
}

void ADESimpleAOEBase::ReturnToPool()
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
