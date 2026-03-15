// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPickup_Heal.h"
#include "DEHealthComponent.h"

ADEPickup_Heal::ADEPickup_Heal()
{
    // 메시 세팅은 블루프린트에서
}

void ADEPickup_Heal::ApplyEffect(AActor* TargetActor)
{
    if (UDEHealthComponent* HealthComp =
        TargetActor->FindComponentByClass<UDEHealthComponent>())
    {
        HealthComp->Heal(Value); // Value = ActivatePickup에서 넘긴 힐량
    }

    Super::ApplyEffect(TargetActor); // DeactivatePickup 호출
}
