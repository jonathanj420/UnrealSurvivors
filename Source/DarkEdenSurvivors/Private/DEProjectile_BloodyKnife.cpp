// Fill out your copyright notice in the Description page of Project Settings.


#include "DEProjectile_BloodyKnife.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DEMonsterBase.h"
#include "DECombatEffect_Execution.h"

// Sets default values
ADEProjectile_BloodyKnife::ADEProjectile_BloodyKnife()
{

    // 1. [전용 메시 로드]
    // 부모가 만든 Mesh 컴포넌트를 가져다 씁니다.
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BKNIFE(TEXT("/Game/DarkEden/Data/Skill/BloodyKnife/SM_BloodyKnife.SM_BloodyKnife"));
    if (SM_BKNIFE.Succeeded())
    {
        Mesh->SetStaticMesh(SM_BKNIFE.Object);
        // 단검 전용 회전값 (눕혀져 있으면 세우기 등)
        Mesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    }

    // 2. [전용 스펙 설정]
    // 부모가 만든 컴포넌트의 수치만 조정합니다.
    if (MovementComponent)
    {
        MovementComponent->InitialSpeed = 1200.0f;
        MovementComponent->MaxSpeed = 1200.0f;
    }

    CollisionComponent->InitSphereRadius(30.0f); // 기본값
  
    /*UDECombatEffect_Execution* Execution =
        CreateDefaultSubobject<UDECombatEffect_Execution>(TEXT("ExecutionEffect"));
    Execution->Threshold = 0.1f;
    Execution->TriggerCondition = ECombatEventTrigger::OnKill;
    LocalEffects.Add(Execution);*/

    // *주의: 델리게이트 연결(AddDynamic)은 부모가 했으니 또 하면 안 됩니다!

    
}

// Called when the game starts or when spawned
void ADEProjectile_BloodyKnife::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADEProjectile_BloodyKnife::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ADEProjectile_BloodyKnife::InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction)
{
    Super::InitializeProjectile(InDamage, InSpeed, InPenetration, Direction);

}

void ADEProjectile_BloodyKnife::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{

    Super::OnOverlap(OverlappedComp,OtherActor,
        OtherComp,OtherBodyIndex,
        bFromSweep,SweepResult);
}