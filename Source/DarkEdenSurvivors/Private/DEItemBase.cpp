// Fill out your copyright notice in the Description page of Project Settings.


#include "DEItemBase.h"
#include "DEFemaleVampire.h"
#include "Components/SphereComponent.h"

// Called when the game starts or when spawned
void ADEItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADEItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ADEItemBase::ADEItemBase()
{
    PrimaryActorTick.bCanEverTick = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->SetSphereRadius(40.f);
    Collision->SetCollisionProfileName("OverlapAllDynamic");

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);

    Collision->OnComponentBeginOverlap.AddDynamic(this, &ADEItemBase::OnOverlap);
}

void ADEItemBase::OnOverlap(UPrimitiveComponent* Overlapped, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    auto Player = Cast<ADEFemaleVampire>(OtherActor);
    if (!Player) return;

    ApplyEffect(Player);

    // 줍으면 풀로 돌아감
    DeactivateItem();
}

void ADEItemBase::ApplyEffect(ADEFemaleVampire* Player)
{
    // 기본은 아무것도 안함
}

void ADEItemBase::ActivateItem(const FVector& Location)
{
    SetActorLocation(Location);
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    SetActorTickEnabled(false);
}

void ADEItemBase::DeactivateItem()
{
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);
}

void ADEItemBase::ResetItem()
{
}
