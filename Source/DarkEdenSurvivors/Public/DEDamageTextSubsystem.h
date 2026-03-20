// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "DEDamageTypes.h"
#include "Subsystems/WorldSubsystem.h"
#include "DEDamageTextSubsystem.generated.h"

class UDEDamageTextWidget;

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEDamageTextSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
    // UWorldSubsystem
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool IsTickableWhenPaused() const override { return false; }

    // 외부(Gameplay) 진입점
    void ShowDamage(const FDamageVisualInfo& Info);

    // Tick
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(
            UDEDamageTextSubsystem, STATGROUP_Tickables);
    }

    virtual bool IsTickable() const override
    {
        // 활성 위젯이 있을 때만 Tick
        return ActiveWidgets.Num() > 0;
    }

private:
    // 내부 유틸
    UDEDamageTextWidget* AcquireWidget();
    APlayerController* GetPlayerController();

private:
    // Widget Class
    UPROPERTY()
    TSubclassOf<UDEDamageTextWidget> DamageTextWidgetClass;

    // 풀 & 활성 목록
    UPROPERTY()
    TArray<UDEDamageTextWidget*> WidgetPool;

    UPROPERTY()
    TArray<UDEDamageTextWidget*> ActiveWidgets;

    // PlayerController 캐시
    TWeakObjectPtr<APlayerController> CachedPC;

    // 초기 풀 크기 (권장 32~64)
    static constexpr int32 InitialPoolSize = 64;
    int32 MaxDamageTexts = 50;
};

