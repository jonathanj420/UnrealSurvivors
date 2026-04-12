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
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("PlayerAttack")); // 기본은 Overlap으로

	//// 2. [메시 생성]
	//Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	//Mesh->SetupAttachment(RootComponent);
	//Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Mesh->SetCastShadow(false);
	//Mesh->SetSimulatePhysics(false);
	//Mesh->SetEnableGravity(false);

	//// 3. [나이아가라 생성]
	//NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	//NiagaraComponent->SetupAttachment(RootComponent);
	//NiagaraComponent->SetAutoActivate(false); // ResetState에서 켬
	//NiagaraComponent->SetRelativeLocation(FVector::ZeroVector);
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
	
	Super::InitializeFromContext(Context);

}

void ADESimpleSummonBase::ResetState()
{
	Super::ResetState();
}

void ADESimpleSummonBase::ReturnToPool()
{
	Super::ReturnToPool();

}
