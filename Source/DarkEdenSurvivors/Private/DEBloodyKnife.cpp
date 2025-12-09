// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBloodyKnife.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DEMonsterBase.h"

// Sets default values
ADEBloodyKnife::ADEBloodyKnife()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    // Collision
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = CollisionComponent;
    CollisionComponent->InitSphereRadius(30.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ADEBloodyKnife::OnOverlap);
    
    // Mesh
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    
    ConstructorHelpers::FObjectFinder<UStaticMesh>SM_BKNIFE(TEXT("/Game/DarkEden/Data/Skill/BloodyKnife/SM_BloodyKnife.SM_BloodyKnife"));
    if (SM_BKNIFE.Succeeded())
    {
        Mesh->SetStaticMesh(SM_BKNIFE.Object);
    }
    Mesh->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f));
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
void ADEBloodyKnife::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADEBloodyKnife::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    LifeTimeCounter += DeltaTime;
    if (LifeTimeCounter >= LifeTime)
    {
        ReturnToPool();
    }

}


void ADEBloodyKnife::InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction)
{
    Super::InitializeProjectile(InDamage, InSpeed, InPenetration, Direction);

}

void ADEBloodyKnife::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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
        Monster->ApplyKnockback(Monster->GetActorLocation() - GetActorLocation(), KnockbackForce);
        if (Penetration <= 0)
        {
            ReturnToPool();
        }
    }
}