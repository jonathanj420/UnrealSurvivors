// Fill out your copyright notice in the Description page of Project Settings.


#include "DEProjectile_AcidBolt.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DEMonsterBase.h"
#include "DEMonsterQuerySubsystem.h"
#include "Engine/World.h"
#include "DEGameplayLibrary.h"
#include "NiagaraComponent.h"
#include "DEAutoSkillBase.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ADEProjectile_AcidBolt::ADEProjectile_AcidBolt()
{
    CollisionComponent->InitSphereRadius(30.0f);
    ConstructorHelpers::FObjectFinder<UStaticMesh>SM_ABOLT(TEXT("/Game/DarkEden/Data/Skill/AcidBolt/AcidBolt.AcidBolt"));
    if (SM_ABOLT.Succeeded())
    {
        Mesh->SetStaticMesh(SM_ABOLT.Object);
    }
    Mesh->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));
    Mesh->SetRelativeLocationAndRotation(FVector(190.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
    MovementComponent->InitialSpeed = 1200.f;
    MovementComponent->MaxSpeed = 1200.f;

    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Custom/AcidBolt2.AcidBolt2")
    );

    if (SoundObj.Succeeded())
    {
        ExplosionSound = SoundObj.Object;
    }
    
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExplosionFX(
        TEXT("/Game/DarkEden/Data/Niagara/NS_AcidBoltExplode.NS_AcidBoltExplode")
    );

    if (ExplosionFX.Succeeded())
    {
        ExplosionEffect = ExplosionFX.Object;
    }


    //Combat
    Damage = 1.0f;
    Penetration = 10;
    KnockbackForce = 1600.0f;
    LifeTime = 3.0f;
    Speed = 1200.0f;
    Size = 30.0f;
    CritChance = 0.5f;
    bCanCrit = false;


}

// Called when the game starts or when spawned
void ADEProjectile_AcidBolt::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void ADEProjectile_AcidBolt::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


}


void ADEProjectile_AcidBolt::OnTargetHit(AActor* Target)
{
    Explode();
}

void ADEProjectile_AcidBolt::Explode()
{
    //UE_LOG(LogTemp, Warning, TEXT("CALLED EXPLODE"));
    UWorld* World = GetWorld();
    if (!World) return;

    FVector Center = GetActorLocation();

    if (const UDEMonsterQuerySubsystem* Query = World->GetSubsystem<UDEMonsterQuerySubsystem>())
    {
        TArray<ADEMonsterBase*> Targets;
        Query->QueryCircle(Center, Radius, Targets);

        for (ADEMonsterBase* Target : Targets)
        {
            if (HitActors.Contains(Target)) continue;
            HitActors.Add(Target);
            TryDealDamage(Target);
        }
    }
    if (ExplosionEffect)
    {
        UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            World,
            ExplosionEffect,
            Center,
            GetActorRotation()
        );

        if (SpawnedEffect)
        {
            // 물리 타격 판정과 완전히 동일한 'EffectRadius'를 나이아가라로 쏴줌!
            SpawnedEffect->SetFloatParameter(FName("SkillRadius"), Radius);
        }
    }
    // ---- 2) 소리 간헐 재생 ----
    static double LastExplosionSoundTime = 0.0;
    const double CurrentTime = FPlatformTime::Seconds();

    // 최소 간격 0.05초
    const double SoundCooldown = 1.0f;

    if (ExplosionSound && (CurrentTime - LastExplosionSoundTime >= SoundCooldown))
    {
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
        LastExplosionSoundTime = CurrentTime;
    }
   // UE_LOG(LogTemp, Warning, TEXT("ACIDBOLT RETURNING TO POOL"));
    ReturnToPool();
}

void ADEProjectile_AcidBolt::OnLifeTimeExpired()
{
    Explode();
}
