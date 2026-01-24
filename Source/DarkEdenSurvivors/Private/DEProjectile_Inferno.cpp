// Fill out your copyright notice in the Description page of Project Settings.


#include "DEProjectile_Inferno.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
ADEProjectile_Inferno::ADEProjectile_Inferno()
{
	Penetration = -1;
	CollisionComponent->InitSphereRadius(90.0f); // 기본값
	if (Mesh)
	{
		Mesh->SetVisibility(false);

	}
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_Fire(TEXT("/Game/DarkEden/Data/Niagara/NS_Inferno.NS_Inferno"));
	if (NS_Fire.Succeeded() && NiagaraComponent)
	{
		NiagaraComponent->SetAsset(NS_Fire.Object);
		// 이펙트 위치 미세 조정 (충돌체 중앙에 오게)
		NiagaraComponent->SetRelativeLocation(FVector::ZeroVector);
		UE_LOG(LogTemp, Warning, TEXT("Inferno niagara set"));
	}
	SetLifeTime(1.0f);
	SetKnockbackForce(100.0f);

}

void ADEProjectile_Inferno::BeginPlay()
{
	Super::BeginPlay();
}

void ADEProjectile_Inferno::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADEProjectile_Inferno::InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction)
{
	Super::InitializeProjectile(InDamage, InSpeed, InPenetration, Direction);
}

void ADEProjectile_Inferno::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
