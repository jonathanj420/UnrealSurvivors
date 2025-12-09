// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAcidBolt.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DEMonsterBase.h"

// Sets default values
ADEAcidBolt::ADEAcidBolt()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // Collision
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = CollisionComponent;
    CollisionComponent->InitSphereRadius(30.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ADEAcidBolt::OnOverlap);

    // Mesh
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

    ConstructorHelpers::FObjectFinder<UStaticMesh>SM_ABOLT(TEXT("/Game/DarkEden/Data/Skill/AcidBolt/AcidBolt.AcidBolt"));
    if (SM_ABOLT.Succeeded())
    {
        Mesh->SetStaticMesh(SM_ABOLT.Object);
    }
    Mesh->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));
    Mesh->SetRelativeLocationAndRotation(FVector(190.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
    Mesh->SetSimulatePhysics(false);
    Mesh->SetEnableGravity(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetupAttachment(RootComponent);
    MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
    MovementComponent->InitialSpeed = 1200.f;
    MovementComponent->MaxSpeed = 1200.f;
    MovementComponent->bRotationFollowsVelocity = true;
    MovementComponent->ProjectileGravityScale = 0.f;
    MovementComponent->SetVelocityInLocalSpace(GetActorForwardVector() * MovementComponent->InitialSpeed);


    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Vampire_Acid_ABolt2.Vampire_Acid_ABolt2")
    );

    if (SoundObj.Succeeded())
    {
        ExplosionSound = SoundObj.Object;
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
    bIsCrit = false;


}

// Called when the game starts or when spawned
void ADEAcidBolt::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void ADEAcidBolt::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    LifeTimeCounter += DeltaTime;
    if (LifeTimeCounter >= LifeTime)
    {
        Explode();
    }

}



void ADEAcidBolt::InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction)
{
    Super::InitializeProjectile(InDamage, InSpeed, InPenetration, Direction);

}

void ADEAcidBolt::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{

    // 1. 이미 이 투사체에 맞았던 액터인지 확인합니다.
    if (HitActors.Contains(OtherActor))
    {
        // 이미 맞은 적이 있다면, 더 이상의 판정 없이 즉시 리턴합니다.
        return;
    }

    if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(OtherActor))
    {
        HitActors.Add(Monster);
        //Monster->OnHitByProjectile(this);
        --Penetration;
        UGameplayStatics::ApplyDamage(Monster, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
        /*if (GetInstigatorController())
        {
            UE_LOG(LogTemp, Warning, TEXT("Instigator Name: %s"), *GetInstigatorController()->GetName());
        }*/

        if (Penetration <= 0)
        {
            Explode();
        }
    }
}

void ADEAcidBolt::Explode()
{
    // Radius = Damage * 5
    UGameplayStatics::ApplyRadialDamage(
        this,
        Damage,
        GetActorLocation(),
        Damage * 5,
        nullptr,
        {},
        this,
        nullptr,
        false
    );
    DrawDebugSphere(GetWorld(), GetActorLocation(), Damage * 5, 16, FColor::Green, 0, 1.0f, 0, 0.0f);
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

    ReturnToPool();
}
