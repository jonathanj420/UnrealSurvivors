// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEDamageTypes.h"
#include "DEHealthComponent.generated.h"

// 체력 변경 (현재 HP, 최대 HP)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, float /*CurrentHP*/, float /*MaxHP*/);


// 사망 이벤트
// Killer: 막타를 친 Instigator (플레이어 Pawn). 치트 등으로 가해자가 없으면 nullptr
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDying, AActor* /*Killer*/);
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
    // 재생이 있을 때만 1초 간격으로 돈다
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // 받는 피해 규칙. 플레이어는 StatComponent가 스탯 값을 넣어 주고, 몬스터는 기본값 0을 쓴다
    // Armor: 피해를 고정량만큼 줄인다. 0이 되지는 않는다 (최소 1, 원래 1보다 작으면 그대로)
    void SetArmor(float InArmor) { Armor = FMath::Max(InArmor, 0.f); }
    // 피해를 받은 뒤 이 시간(초) 동안 모든 피해를 무시한다
    void SetInvulnerabilityTime(float InSeconds) { InvulnerabilityTime = FMath::Max(InSeconds, 0.f); }
    // 초당 회복량
    void SetRegeneration(float InPerSecond);

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

    // 사망은 두 단계다. 소유 액터는 OnDying에 반응해 죽음을 처리한다.
    // OnDying: HP가 0이 된 순간 HealthComponent가 broadcast한다
    FOnDying OnDying;
    // OnDeath: 소유 액터가 사망 처리를 끝냈을 때 소유 액터가 broadcast한다 (몬스터는 ExecuteFinalDeath). HealthComponent는 부르지 않는다
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

    UPROPERTY(VisibleInstanceOnly, Category = "Health|Defense")
    float Armor = 0.f;

    UPROPERTY(VisibleInstanceOnly, Category = "Health|Defense")
    float InvulnerabilityTime = 0.f;

    UPROPERTY(VisibleInstanceOnly, Category = "Health|Defense")
    float Regeneration = 0.f;

    // 월드 시간 기준. 일시정지 중에는 흐르지 않는다
    double InvulnerableUntil = 0.0;

private:
    void HandleDeath(AActor* Killer);
public:
    UFUNCTION(BlueprintCallable, Category = "Combat")
    FDEDamageResult ProcessDamage(const FDEDamageRequest& Request);

    // 데미지 공식을 무시하고 즉시 처형 (Execute)
    UFUNCTION(BlueprintCallable, Category = "Health")
    void InstantKill(AActor* Executioner, bool bShowDamage /* = false */);
protected:
    void ApplyFinalDamage(float InDamage, AActor* InInstigator, bool bInIsCritical, const FGameplayTagContainer& InDamageTags);

public:
    // 기본 타격음 (살점 맞는 소리 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* DefaultHitSound;

		
};
