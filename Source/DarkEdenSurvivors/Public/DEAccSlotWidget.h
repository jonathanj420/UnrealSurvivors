// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "DEAccSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEAccSlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 스킬 슬롯의 SetSkill과 완벽히 대응되는 함수
    UFUNCTION(BlueprintCallable)
    void SetAccessory(const FText& InAccName, int32 InAccLevel, UTexture2D* InAccIcon);

protected:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* AccNameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* AccLevelText;

    UPROPERTY(meta = (BindWidget))
    UImage* AccIconImage;

    FText AccName;
    int32 AccLevel;
	
};
