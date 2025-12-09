// Fill out your copyright notice in the Description page of Project Settings.


#include "DESimpleProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DEPoolSubsystem.h"
#include "Subsystems/WorldSubsystem.h"

// Sets default values
ADESimpleProjectileBase::ADESimpleProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
 
}

// Called when the game starts or when spawned
void ADESimpleProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADESimpleProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ADESimpleProjectileBase::ResetState()
{
	HitActors.Empty();
	LifeTimeCounter = 0.f;
}

void ADESimpleProjectileBase::ReturnToPool()
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

void ADESimpleProjectileBase::InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction)
{
	ResetState();

	Damage = InDamage;
	Speed = InSpeed;
	Penetration = InPenetration;

	MovementComponent->Velocity = Direction * Speed;
}

void ADESimpleProjectileBase::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{

}
void ADESimpleProjectileBase::OnLifeTimeExpired()
{
	ReturnToPool();
}