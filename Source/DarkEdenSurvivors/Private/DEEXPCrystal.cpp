// Fill out your copyright notice in the Description page of Project Settings.


#include "DEEXPCrystal.h"
#include "Components/SphereComponent.h"
#include "DEFemaleVampire.h"

// Sets default values
ADEEXPCrystal::ADEEXPCrystal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = Collision;
	Collision->SetSphereRadius(40.f);
	Collision->SetCollisionProfileName("OverlapAllDynamic");

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_EXPCrystal(TEXT("/Game/DarkEden/Data/StaticMesh/EXPCrystal/EXPCrystal.EXPCrystal"));
	if (SM_EXPCrystal.Succeeded())
	{

		Mesh->SetStaticMesh(SM_EXPCrystal.Object);
	}
	Mesh->SetupAttachment(RootComponent);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ADEEXPCrystal::OnOverlap);
	//RootComponent->SetWorldScale3D(FVector(0.1f));
	Mesh->SetRelativeScale3D(FVector(0.1f));
}

// Called when the game starts or when spawned
void ADEEXPCrystal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADEEXPCrystal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	// 거리 따라 빨려오는 속도 증가
	float Dist = FVector::Dist(Player->GetActorLocation(), GetActorLocation());
	float CurSpeed = 50.0f;

	if (Dist < 300.f)
		CurSpeed = 500.f;
	if (Dist < 150.f)
		CurSpeed = 900.f;

	FVector Dir = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	AddActorWorldOffset(Dir * CurSpeed * DeltaTime, true);


}

void ADEEXPCrystal::OnOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	ADEFemaleVampire* Char = Cast<ADEFemaleVampire>(OtherActor);
	if (Char)
	{
		Char->AddEXP(EXPAmount);
		Destroy();
	}
}