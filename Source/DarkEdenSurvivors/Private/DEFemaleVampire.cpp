// Fill out your copyright notice in the Description page of Project Settings.


#include "DEFemaleVampire.h"
#include "DEBloodyKnife.h"
#include "DEStatComponent.h"
#include "DESkillManagerComponent.h"

// Sets default values
ADEFemaleVampire::ADEFemaleVampire()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_COUNTESS(TEXT("/Game/ParagonCountess/Characters/Heroes/Countess/Meshes/SM_Countess.SM_Countess"));
	if (SM_COUNTESS.Succeeded())
	{

		GetMesh()->SetSkeletalMesh(SM_COUNTESS.Object);
	}
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	BaseSkillID = 4;
}

// Called when the game starts or when spawned
void ADEFemaleVampire::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADEFemaleVampire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
   
}
