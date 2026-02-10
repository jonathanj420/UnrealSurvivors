// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DEDamageTypes.h"
#include "DEDamageTextWidget.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEDamageTextWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void Activate(const FDamageVisualInfo& Info);
    void Deactivate();

    bool IsActive() const { return bActive; }
    FVector GetWorldLocation() const { return TargetWorldLocation; }

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* DamageText;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    class UWidgetAnimation* PopAnim;

    UFUNCTION()
    void HandleAnimFinished();

private:
    FVector TargetWorldLocation;
    bool bActive = false;
	
};
