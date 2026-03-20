// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DESkillInventoryWidget.generated.h"

class UDEInventoryComponent;
class UDESkillManagerComponent;
class UPanelWidget;
class UDESkillSlotWidget;


/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDESkillInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UDESkillInventoryWidget(const FObjectInitializer& ObjectInitializer);
protected:
    virtual void NativeConstruct() override;

    void Refresh();
    void OnSkillUpdated(int32 InSkillID);

public:
    // Pawn을 외부에서 넣어주는 초기화 함수
    void InitializeSkillWidget(APawn* NewPawn);

protected:
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* SkillSlotPanel; // HorizontalBox / UniformGrid

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UDESkillSlotWidget> SkillSlotWidgetClass;

private:
    UPROPERTY()
    UDEInventoryComponent* Inventory;

    UPROPERTY()
    UDESkillManagerComponent* SkillManager;

};
