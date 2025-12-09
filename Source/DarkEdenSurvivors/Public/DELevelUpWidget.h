// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DESkillManagerComponent.h"
#include "DELevelUpWidget.generated.h"


class UButton;
class UTextBlock;


/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDELevelUpWidget : public UUserWidget
{

    GENERATED_BODY()

public:
    void InitWidget(UDESkillManagerComponent* InSkillManager);

protected:
    virtual void NativeConstruct() override;

protected:

    UPROPERTY(meta = (BindWidget))
    UButton* Btn1;
    UPROPERTY(meta = (BindWidget))
    UButton* Btn2;
    UPROPERTY(meta = (BindWidget))
    UButton* Btn3;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt1;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt2;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Txt3;

    UDESkillManagerComponent* SkillManager;

    // 선택된 3개
    TArray<FDESkillData*> Selected;

    UFUNCTION()
    void OnSelect1();
    UFUNCTION()
    void OnSelect2();
    UFUNCTION()
    void OnSelect3();
    
};
