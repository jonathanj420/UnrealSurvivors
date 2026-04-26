// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_PlayUI.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_PlayUI : public UDESkillBehavior
{
    GENERATED_BODY()

public:
    // 기획자가 띄울 UI 클래스를 선택할 수 있게 뚫어줍니다.
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> CinematicWidgetClass;

    // UI가 다른 모든 것들을 덮도록 Z-Order 설정 (기본값 100)
    UPROPERTY(EditAnywhere, Category = "UI")
    int32 ZOrder = 100;

    virtual void Execute(FDESkillContext& Context) override;
	
};
