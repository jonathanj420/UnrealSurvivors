// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UDEStatComponent::UDEStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// [기본값 설정]
	MoveSpeed = FGameplayStat(600.0f); // 언리얼 기본 달리기 속도
	MagnetRange = FGameplayStat(100.0f); // 기본 줍기 범위
	Luck = FGameplayStat(1.0f);
	Greed = FGameplayStat(1.0f);
	Curse = FGameplayStat(1.0f);
}


// Called when the game starts
void UDEStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작 시 초기 스탯 적용
	UpdateMovementSpeed();
	UpdateMagnetRange();
	
}


// Called every frame
void UDEStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}


void UDEStatComponent::UpdateMovementSpeed()
{
	// 1. 최종 값 계산
	float FinalSpeed = MoveSpeed.GetValue();

	// 2. 캐릭터 무브먼트에 적용
	if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* Movement = OwnerChar->GetCharacterMovement())
		{
			Movement->MaxWalkSpeed = FinalSpeed;
		}
	}

	// 3. 이벤트 전파 (필요하다면)
	if (OnSpeedChanged.IsBound())
	{
		OnSpeedChanged.Broadcast(FinalSpeed);
	}
}

void UDEStatComponent::UpdateMagnetRange()
{
	float FinalRange = MagnetRange.GetValue();

	// 플레이어 캐릭터에 자석용 SphereComponent가 있다고 가정하고 찾아서 적용
	// (Tag를 "Magnet"으로 해두거나, 캐릭터 클래스에서 바인딩해서 처리)
	if (AActor* Owner = GetOwner())
	{
		// 방법 A: 태그로 찾아서 크기 조절 (느슨한 결합)
		TArray<UActorComponent*> Comps = Owner->GetComponentsByTag(USphereComponent::StaticClass(), TEXT("Magnet"));
		if (Comps.Num() > 0)
		{
			if (USphereComponent* MagnetSphere = Cast<USphereComponent>(Comps[0]))
			{
				MagnetSphere->SetSphereRadius(FinalRange);
			}
		}
	}

	// 방법 B: 그냥 알리기만 하고 캐릭터가 알아서 하게 함 (추천)
	if (OnMagnetChanged.IsBound())
	{
		OnMagnetChanged.Broadcast(FinalRange);
	}
}

void UDEStatComponent::ResetStats()
{
	// 모든 Modifier(버프/디버프) 제거 후 초기화
	MoveSpeed.ResetModifiers();
	MagnetRange.ResetModifiers();
	Luck.ResetModifiers();
	Greed.ResetModifiers();
	Curse.ResetModifiers();

	// 적용
	UpdateMovementSpeed();
	UpdateMagnetRange();
}