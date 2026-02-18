// Fill out your copyright notice in the Description page of Project Settings.


#include "DELevelUpChoice_Accessory.h"
#include "DEAccessoryData.h"
#include "DEAccessoryComponent.h"
#include "GameFramework/Actor.h"

void UDELevelUpChoice_Accessory::Init(UDEAccessoryData* InAccessoryData)
{
    AccessoryData = InAccessoryData;
}

FText UDELevelUpChoice_Accessory::GetDisplayName() const
{
    if (!AccessoryData)
        return FText::FromString(TEXT("Invalid Accessory"));

    return AccessoryData->Name;
}

FText UDELevelUpChoice_Accessory::GetDescription() const
{
    if (!AccessoryData)
        return FText::GetEmpty();

    return AccessoryData->Description;
}

UTexture2D* UDELevelUpChoice_Accessory::GetIcon() const
{
    if (!AccessoryData)
        return nullptr;

    return AccessoryData->Icon;
}

void UDELevelUpChoice_Accessory::Apply(AActor* PlayerActor)
{
    if (!PlayerActor || !AccessoryData)
        return;

    if (UDEAccessoryComponent* AccComp =
        PlayerActor->FindComponentByClass<UDEAccessoryComponent>())
    {
        AccComp->AddAccessory(AccessoryData);
    }
}