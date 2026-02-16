// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DEGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ADEGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADEGameModeBase();

	// 1. 플레이어 접속/퇴장 감지 (엔진 기본 함수 오버라이드)
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	// 2. 자식들이 쓸 유틸리티 (저장/로드 셔틀)
	UFUNCTION(BlueprintCallable, Category = "System")
	void SaveGameData();

	// 치트키 (콘솔 명령어로 실행 가능하게)
	UFUNCTION(Exec)
	void Cheat_AddGold(int32 Amount);

};
