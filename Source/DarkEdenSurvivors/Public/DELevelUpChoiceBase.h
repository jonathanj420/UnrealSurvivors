// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DELevelUpChoiceBase.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDELevelUpChoiceBase : public UObject
{
    GENERATED_BODY()

public:
    virtual FText GetDisplayName() const PURE_VIRTUAL(UDELevelUpChoiceBase::GetDisplayName, return FText(););
    virtual FText GetDescription() const PURE_VIRTUAL(UDELevelUpChoiceBase::GetDescription, return FText(););
    virtual UTexture2D* GetIcon() const PURE_VIRTUAL(UDELevelUpChoiceBase::GetIcon, return nullptr;);

    // ★ 핵심: 선택 시 실제 적용
    virtual void Apply(AActor * PlayerActor) PURE_VIRTUAL(UDELevelUpChoiceBase::Apply, );
	
};
