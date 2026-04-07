// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DEPauseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEPauseMenuWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;
    virtual void NativeOnInitialized() override;
    // 블루프린트 버튼과 이름이 똑같아야 매핑됨!
    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Resume;
    UPROPERTY(meta = (BindWidget))
    class UButton* Btn_Exit;


    UFUNCTION()
    void OnResumeClicked();
    UFUNCTION()
    void OnExitClicked();

	
};
