// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAccSlotWidget.h"


void UDEAccSlotWidget::SetAccessory(const FText& InAccName, int32 InAccLevel, UTexture2D* InAccIcon)
{
    AccName = InAccName;
    AccLevel = InAccLevel;

    // 1. 이름 세팅
    if (AccNameText)
    {
        AccNameText->SetText(AccName);
    }

    // 2. 레벨 텍스트 세팅 (예: "Lv.1")
    if (AccLevelText)
    {
        FString LevelStr = FString::Printf(TEXT("Lv.%d"), AccLevel);
        AccLevelText->SetText(FText::FromString(LevelStr));
    }

    // 3. 아이콘 이미지 세팅
    if (AccIconImage && InAccIcon)
    {
        AccIconImage->SetBrushFromTexture(InAccIcon);
    }
}