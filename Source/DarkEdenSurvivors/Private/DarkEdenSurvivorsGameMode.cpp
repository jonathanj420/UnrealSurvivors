// Copyright Epic Games, Inc. All Rights Reserved.

#include "DarkEdenSurvivorsGameMode.h"

void ADarkEdenSurvivorsGameMode::StartPlay()
{

    Super::StartPlay();

    check(GEngine != nullptr);

    // 디버그 메시지를 5초간 표시합니다. 
    // -1 '키' 값 실행인자가 메시지가 업데이트되거나 새로고침되지 않도록 방지합니다.
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Hello World, this is FPSGameModeBase!"));

}

ADarkEdenSurvivorsGameMode::ADarkEdenSurvivorsGameMode()
{
    

}
void ADarkEdenSurvivorsGameMode::SpawnMonsters(TSubclassOf<APawn> MonsterClass, int32 Count, FVector CenterLocation, float Radius)
{
    if (!MonsterClass || !GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("SpawnMonsters: Invalid MonsterClass or World."));
        return;
    }

    for (int32 i = 0; i < Count; ++i)
    {
        // 1. 스폰할 위치를 랜덤하게 계산합니다.
        // 특정 반경(Radius) 내에서 랜덤한 위치를 구합니다.
        float RandAngle = FMath::RandRange(0.0f, 360.0f);
        float RandDistance = FMath::RandRange(0.0f, Radius);

        float XOffset = FMath::Cos(FMath::DegreesToRadians(RandAngle)) * RandDistance;
        float YOffset = FMath::Sin(FMath::DegreesToRadians(RandAngle)) * RandDistance;

        // CenterLocation을 기준으로 오프셋을 적용합니다. (Z는 바닥에 고정)
        FVector SpawnLocation = CenterLocation + FVector(XOffset, YOffset, 0.0f);

        // 2. 스폰 회전값 설정 (Z축을 위로)
        FRotator SpawnRotation = FRotator::ZeroRotator;

        // 3. 몬스터 스폰
        // FActorSpawnParameters를 사용하여 충돌 무시하고 스폰
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        APawn* NewMonster = GetWorld()->SpawnActor<APawn>(MonsterClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (NewMonster)
        {
            UE_LOG(LogTemp, Warning, TEXT("Spawned Monster: %s at %s"), *NewMonster->GetName(), *SpawnLocation.ToString());
        }
    }
}