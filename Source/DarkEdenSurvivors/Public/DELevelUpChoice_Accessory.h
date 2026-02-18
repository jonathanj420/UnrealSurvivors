// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DELevelUpChoiceBase.h"
#include "DELevelUpChoice_Accessory.generated.h"

class UDEAccessoryData;
class UTexture2D;


/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDELevelUpChoice_Accessory : public UDELevelUpChoiceBase
{
    GENERATED_BODY()

public:
    void Init(UDEAccessoryData* InAccessoryData);

    // ===== UDELevelUpChoice =====
    virtual FText GetDisplayName() const override;
    virtual FText GetDescription() const override;
    virtual UTexture2D* GetIcon() const override;
    virtual void Apply(AActor* PlayerActor) override;

private:
    UPROPERTY()
    UDEAccessoryData* AccessoryData = nullptr;
	
};
