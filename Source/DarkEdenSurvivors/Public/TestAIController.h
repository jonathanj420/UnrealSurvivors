// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DarkEdenSurvivors.h"
#include "AIController.h"
#include "TestAIController.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API ATestAIController : public AAIController
{
    GENERATED_BODY()

public:
    ATestAIController();

	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;

    // 매 프레임 실행되어 플레이어 위치를 갱신합니다.
    virtual void Tick(float DeltaSeconds) override;
};
