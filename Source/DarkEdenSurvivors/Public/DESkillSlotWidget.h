// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "DESkillSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // [변경] 입력받는 ID도 숫자가 아니라 이름(FName)이어야 합니다.
    UFUNCTION(BlueprintCallable)
    void SetSkill(const FText& InSkillName, int32 InSkillLevel, UTexture2D* InSkillIcon);

protected:
    // 실제 UI 위젯들 (BP에서 BindWidget)
    UPROPERTY(meta = (BindWidget))
    UTextBlock* SkillNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SkillLevelText;

    UPROPERTY(meta = (BindWidget))
    UImage* SkillIconImage;

    FText SkillName;
    int32 SkillLevel;
};
