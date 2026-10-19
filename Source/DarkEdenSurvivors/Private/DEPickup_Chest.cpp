// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPickup_Chest.h"
#include "DELevelUpManagerComponent.h"

ADEPickup_Chest::ADEPickup_Chest()
{
    // 1. 상자는 무거워서 자석에 안 끌려갑니다!
    bCanBeMagnetized = false;

    // (선택) 상자는 덩치가 크니까 경험치 구슬보다 먹는 범위를 넉넉하게 줍니다.
    SetTriggerRadius(40.0f);

}

void ADEPickup_Chest::ApplyEffect(AActor* TargetActor)
{
    // 1. 기본 픽업 이펙트(소리, 파티클 등) 실행
    //Super::ApplyEffect(TargetActor);

    // 2. 플레이어의 레벨업 매니저 가져오기
    UDELevelUpManagerComponent* LevelUpMgr = TargetActor->FindComponentByClass<UDELevelUpManagerComponent>();
    if (!LevelUpMgr) return;

    // 3. (임시) 행운 스탯. 나중에 TargetActor에서 제대로 가져오기
    float PlayerLuck = 0.0f;

    // 4. 보상 생성과 적용은 매니저가 한다. 일시정지와 UI는 매니저 신호를 받은 컨트롤러가 한다
    LevelUpMgr->EnqueueChest(PlayerLuck);

    Super::ApplyEffect(TargetActor);
}