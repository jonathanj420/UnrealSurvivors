// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPlayerAnimInstance.h"
#include "DECharacterBase.h"

void UDEPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<ADECharacterBase>(TryGetPawnOwner());
}

void UDEPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (PlayerCharacter)
	{
		CurrentSpeed = PlayerCharacter->GetVelocity().Size2D();

		bIsMoving = (CurrentSpeed > 1.0f);
	}
}