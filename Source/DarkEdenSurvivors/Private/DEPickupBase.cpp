// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPickupBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DEPoolSubsystem.h"
#include "DEPickupManager.h"
#include "Engine/World.h"

// Sets default values
ADEPickupBase::ADEPickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	//Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 플레이어(Pawn)만 감지!
	Trigger->SetSphereRadius(30.0f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCastShadow(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메쉬 충돌 끔
}

// Called when the game starts or when spawned
void ADEPickupBase::BeginPlay()
{
	Super::BeginPlay();
	if (Trigger)
	{
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &ADEPickupBase::OnOverlapBegin);
	}
}


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
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//SetActorTickEnabled(false); // 이동 전까진 틱 끔
}

void ADEPickupBase::DeactivatePickup()
{
	// 끄기
	SetActorHiddenInGame(true);
	Trigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorTickEnabled(false);

	// 매니저 장부에서 빼고 풀에 반환
	if (UDEPickupManager* PickupMgr = GetWorld()->GetSubsystem<UDEPickupManager>())
	{
		PickupMgr->UnregisterPickup(this);
	}
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

void ADEPickupBase::SetTriggerRadius(float InSize)
{
	if (Trigger)
	{
		Trigger->SetSphereRadius(InSize);
	}
	

}

void ADEPickupBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Pickup Touched Somthin,...."));
	// 이미 먹혀서 안 보이는 놈이거나, OtherActor가 없으면 무시
	if (IsHidden())
	{
		UE_LOG(LogTemp, Warning, TEXT("WTF?"));
	}
	if (IsHidden() || !OtherActor) return;

	// (선택) OtherActor가 진짜 플레이어인지 검사 (태그 기반이 젤 편합니다)
	if (OtherActor->ActorHasTag(TEXT("Player")))
	{
		// 닿았다! 바로 효과 적용!
		ApplyEffect(OtherActor);

		// ApplyEffect → DeactivatePickup에서 매니저 장부 제거와 풀 반환까지 처리된다
	}
}
