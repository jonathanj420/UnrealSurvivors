// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPickup_Chest.h"
#include "DELevelUpManagerComponent.h"
#include "DESkillManagerComponent.h"
#include "DEPlayerController.h"
#include "Kismet/GameplayStatics.h"

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

    // 4. 네가 짠 잭팟 룰렛 돌려서 보상 목록(1, 3, 5개) 뽑아오기!
    TArray<UDELevelUpChoiceBase*> ChestRewards = LevelUpMgr->GenerateChestRewards(PlayerLuck);

    // 5. 게임 시간 즉시 멈춰! (타이머들도 다 같이 멈춤)
    ADEPlayerController* PC = Cast<ADEPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (PC)
    {
        PC->SetPause(true);
        PC->ShowChestWidget(ChestRewards);
    }
    Super::ApplyEffect(TargetActor);
}