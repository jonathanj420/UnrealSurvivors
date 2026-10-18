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
