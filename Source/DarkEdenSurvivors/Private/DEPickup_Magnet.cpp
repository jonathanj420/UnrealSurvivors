// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPickup_Magnet.h"
#include "DEPickupManager.h"

ADEPickup_Magnet::ADEPickup_Magnet()
{
	bCanBeMagnetized = false;

}

void ADEPickup_Magnet::ApplyEffect(AActor* TargetActor)
{
	if (!TargetActor) return;

	// 1. 매니저 서브시스템 가져오기
	UDEPickupManager* PickupMgr = GetWorld()->GetSubsystem<UDEPickupManager>();
	if (PickupMgr)
	{
		// 2. 만들어두신 글로벌 자석 발동 함수 호출!
		PickupMgr->TriggerGlobalMagnet(TargetActor);
		UE_LOG(LogTemp, Warning, TEXT("Magnet On Action"));
	}

	// 3. 기본 사운드/파티클 재생 및 풀(Pool)에 자기 자신 반환
	Super::ApplyEffect(TargetActor);
}
