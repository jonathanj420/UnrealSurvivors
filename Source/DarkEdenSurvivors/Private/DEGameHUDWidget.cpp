// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameHUDWidget.h"
#include "Components/TextBlock.h"


void UDEGameHUDWidget::UpdateTime(float TimeSeconds)
{
    int32 Minutes = TimeSeconds / 60;
    int32 Seconds = (int32)TimeSeconds % 60;

    FString TimeStr = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

    if (Text_Time)
        Text_Time->SetText(FText::FromString(TimeStr));
}