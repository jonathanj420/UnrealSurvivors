// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DEAccInventoryWidget.generated.h"

class UDEAccessoryComponent;
class UPanelWidget;
class UDEAccSlotWidget;

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEAccInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    void Refresh();
    // 스킬은 ID를 썼지만, 악세서리는 DataAsset 포인터를 쓰면 됩니다.
    void OnAccessoryUpdated(const class UDEAccessoryData* InAccData);

public:
    // Pawn을 외부에서 넣어주는 초기화 함수 (스킬과 동일)
    void InitializeAccessoryWidget(APawn* NewPawn);

protected:
    // 스킬 패널과 동일하게 HorizontalBox나 UniformGrid를 받을 수 있는 상위 클래스
    UPROPERTY(meta = (BindWidget))
    UPanelWidget* AccSlotPanel;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UDEAccSlotWidget> AccSlotWidgetClass;

private:
    // 스킬 매니저 대신 악세서리 컴포넌트를 들고 있습니다.
    UPROPERTY()
    UDEAccessoryComponent* AccessoryComp;
	
};
