// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPickupEXPGem.h"
#include "DEProgressionComponent.h"
ADEPickupEXPGem::ADEPickupEXPGem()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_EXPGem(TEXT("/Game/DarkEden/Data/StaticMesh/EXPCrystal/EXPCrystal.EXPCrystal"));
	if (SM_EXPGem.Succeeded())
	{
		Mesh->SetStaticMesh(SM_EXPGem.Object);
	}
    Mesh->SetRelativeScale3D(FVector(0.1f));
	bCanBeMagnetized = true;
}

void ADEPickupEXPGem::ApplyEffect(AActor* TargetActor)
{
    // 1. 방어 코드: TargetActor가 없을 수도 있음 (안전 제일)
    if (!TargetActor) return;

    // 2. 경험치 컴포넌트 찾기
    UDEProgressionComponent* Prog = TargetActor->FindComponentByClass<UDEProgressionComponent>();

    // 3. 컴포넌트가 있을 때만 경험치 지급 (없는데 쓰면 크래시!)
    if (Prog)
    {
        Prog->AddEXP(Value);
    }

    // 4. ★ 부모 호출 (여기서 DeactivatePickup이 실행되어 풀로 돌아감)
    Super::ApplyEffect(TargetActor);
}
