// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPickupBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DEPoolSubsystem.h"

// Sets default values
ADEPickupBase::ADEPickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Trigger->SetSphereRadius(40.0f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메쉬 충돌 끔
}

// Called when the game starts or when spawned
void ADEPickupBase::BeginPlay()
{
	Super::BeginPlay();
	//Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADEPickupBase::OnOverlap);
}

// Called every frame
//void ADEPickupBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//	if (bIsMagnetized && MagnetTarget)
//	{
//		FVector MyLoc = GetActorLocation();
//		FVector TargetLoc = MagnetTarget->GetActorLocation();
//		TargetLoc.Z += 50.0f; // 허리 높이로 유도
//
//		// 거리 체크 (너무 가까우면 강제 습득)
//		float DistSq = FVector::DistSquared(MyLoc, TargetLoc);
//		if (DistSq < 1000.0f) // 약 30cm
//		{
//			ApplyEffect(MagnetTarget);
//			return;
//		}
//
//		// 이동 로직
//		FVector Dir = (TargetLoc - MyLoc).GetSafeNormal();
//		CurrentSpeed += (Acceleration * DeltaTime); // 가속
//		SetActorLocation(MyLoc + (Dir * CurrentSpeed * DeltaTime));
//	}
//	else
//	{
//		SetActorTickEnabled(false); // 타겟 없으면 멈춤
//	}
//}

void ADEPickupBase::ActivatePickup(const FVector& Location, float DataValue)
{
	SetActorLocation(Location);
	Value = DataValue; // 경험치 10, 체력 30 등 값 설정

	// 상태 초기화
	bIsMagnetized = false;
	MagnetTarget = nullptr;
	CurrentSpeed = 0.0f; // 속도 리셋

	// 켜기
	SetActorHiddenInGame(false);
	//SetActorEnableCollision(true);
	//SetActorTickEnabled(false); // 이동 전까진 틱 끔
}

void ADEPickupBase::DeactivatePickup()
{
	// 끄기
	SetActorHiddenInGame(true);
	//SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	// TODO: 풀 시스템에 반환 요청
	GetWorld()->GetSubsystem<UDEPoolSubsystem>()->ReturnActorToPool(this);

	// 풀 시스템 없으면 임시 파괴
	// Destroy();
}



void ADEPickupBase::ApplyEffect(AActor* TargetActor)
{
	// 부모는 아무것도 안 함 (가상함수)
	// 자식이 override 해서 경험치를 주든 힐을 하든 함

	// 효과 적용 후 사라짐
	DeactivatePickup();
}