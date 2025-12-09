// Fill out your copyright notice in the Description page of Project Settings.


#include "TestAIController.h"
#include "Kismet/GameplayStatics.h"

ATestAIController::ATestAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ATestAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
}

void ATestAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 1. 플레이어 폰을 가져옵니다.
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (PlayerPawn)
    {
        // 2. 플레이어를 향해 이동 명령을 내립니다.
        // 추격 거리 1.0f: 플레이어에게 아주 가깝게 붙으라는 의미입니다.
        MoveToActor(PlayerPawn, 1.0f);
    }
}