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

    //AActor* GetTargetActor() const { return TargetActor; }

    // 대망의 누적 데미지 처리 함수!
    void AddDamage(float AdditionalDamage);


protected:
    virtual void NativeConstruct() override;

protected:
    // 방금 UMG에서 감싸준 부모 컨테이너를 코드로 가져옵니다.
    UPROPERTY(meta = (BindWidget))
    class UOverlay* ScaleContainer;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* DamageText;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    //class UWidgetAnimation* PopAnim;
    class UWidgetAnimation* PopAnimMk2;

    UFUNCTION()
    void HandleAnimFinished();

private:
    FVector TargetWorldLocation;
    bool bActive = false;
	
};
