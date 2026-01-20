// Fill out your copyright notice in the Description page of Project Settings.


#include "DESimpleProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DEPoolSubsystem.h"
#include "DESkillContext.h"

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
	if (InSpeed == 0.0f)
	{
		Speed = 1200.0f;

	}
	else
	{
		Speed = InSpeed;
	}
	Penetration = InPenetration;

	MovementComponent->Velocity = Direction * Speed;
}

void ADESimpleProjectileBase::InitializeFromContext(const FDESkillContext& Context, const FVector& Direction)
{
	// 1. 메인 스탯 적용
	InitializeProjectile(Context.Damage, Context.Speed, Context.Penetration, Direction);

	// 2. 확장 스탯 적용 (Map에서 꺼내오기)
	// "KnockbackForce" 키가 있으면 적용, 없으면 0
	float KForce = Context.KnockbackForce;
	SetKnockbackForce(KForce);

	// "Radius"나 "Size" 키가 있다면?
	float NewSize = Context.GetValue(TEXT("Size"), -1.f);
	if (NewSize > 0.f)
	{
		SetSize(NewSize);
		// 여기서 실제 Mesh 스케일 조절 로직 호출
	}

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