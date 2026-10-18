// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPickup_Gold.h"
#include "DEGameMode_Stage.h"
#include "DEStatComponent.h"
#include "Engine/World.h"

ADEPickup_Gold::ADEPickup_Gold()
{
    // 메시 세팅은 블루프린트에서
    bCanBeMagnetized = true;
}

void ADEPickup_Gold::ApplyEffect(AActor* TargetActor)
{
    if (!TargetActor) return;

    // Greed는 주운 캐릭터 기준 (기본 1.0, 메타 업그레이드로 증가)
    float GoldMultiplier = 1.0f;
    if (UDEStatComponent* StatComp = TargetActor->FindComponentByClass<UDEStatComponent>())
    {
        GoldMultiplier = StatComp->GetGreed();
    }

    // 골드 장부는 GameMode가 관리 (서버 권한)
    if (ADEGameMode_Stage* StageGM = GetWorld()->GetAuthGameMode<ADEGameMode_Stage>())
    {
        StageGM->AddGold(FMath::RoundToInt(Value * GoldMultiplier));
    }

    Super::ApplyEffect(TargetActor); // DeactivatePickup 호출
}
