// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEDamageTypes.h"
#include "DEHealthComponent.generated.h"

// 체력 변경 (현재 HP, 최대 HP)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float /*CurrentHP*/, float /*MaxHP*/);

// 사망 이벤트
DECLARE_MULTICAST_DELEGATE(FOnDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDEHealthComponent();

protected:
    virtual void BeginPlay() override;

public:
    // =====================
    // Damage / Heal API
    // =====================

    //void ApplyDamage(float DamageAmount, AActor* DamageCauser = nullptr);
    void Heal(float HealAmount);
    void ResetHealth(bool bHealToFull = true); //for pooling

    // =====================
    // Getters
    // =====================

    float GetCurrentHP() const { return CurrentHP; }
    float GetMaxHP() const { return MaxHP; }
    bool IsAlive() const { return !bIsDead && CurrentHP > 0.f; }
    bool IsDead() const { return bIsDead; }
    // =====================
    // Max HP Control
    // =====================

    void SetMaxHP(float NewMaxHP, bool bHealToFull = false);
    void IncreaseMaxHP(float Amount, bool bHealToFull = false);
    void SetCurrentHP(float NewHP);

public:
    // =====================
    // Events
    // =====================

    FOnHPChanged OnHPChanged;
    FOnDeath OnDeath;

private:
    // =====================
    // Internal State
    // =====================

    UPROPERTY(EditAnywhere, Category = "Health")
    float MaxHP = 100.f;

    UPROPERTY(VisibleAnywhere, Category = "Health")
    float CurrentHP = 100.f;

    bool bIsDead = false;

private:
    void HandleDeath(AActor* DamageCauser);
public:
    UFUNCTION(BlueprintCallable, Category = "Combat")
    FDEDamageResult ProcessDamage(const FDEDamageRequest& Request);

    // 데미지 공식을 무시하고 즉시 처형 (Execute)
    UFUNCTION(BlueprintCallable, Category = "Health")
    void InstantKill(AActor* Executioner, bool bShowDamage /* = false */);
protected:
    void ApplyFinalDamage(float InDamage, AActor* InCauser, bool bInIsCritical, const FGameplayTagContainer& InDamageTags);

public:
    // 기본 타격음 (살점 맞는 소리 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* DefaultHitSound;

		
};
