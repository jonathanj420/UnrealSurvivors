// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPickup_Chest.h"

ADEPickup_Chest::ADEPickup_Chest()
{
    // 1. 상자는 무거워서 자석에 안 끌려갑니다!
    bCanBeMagnetized = false;

    // (선택) 상자는 덩치가 크니까 경험치 구슬보다 먹는 범위를 넉넉하게 줍니다.
    SetTriggerRadius(40.0f);

}

void ADEPickup_Chest::ApplyEffect(AActor* TargetActor)
{
    // 1. 안전 검사
    if (!TargetActor) return;

    UE_LOG(LogTemp, Warning, TEXT("Chest"));

    // 2. 게임 일시정지! (시간이 멈추고 몹들이 멈춰야 함)
    //// GetWorld()의 타이머 등은 안 멈추게 하려면 UI 띄울 때 SetGamePaused를 쓰는 게 정석입니다.
    //APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    //if (PC)
    //{
    //    // 나중에 UI 띄우는 함수 호출
    //    // PC->ShowChestRouletteUI();
    //}

    // 3. 다 먹었으니 맵에서 지우기 (Destroy 대신 님이 만든 풀링 회수 함수 호출!)
    DeactivatePickup();
}