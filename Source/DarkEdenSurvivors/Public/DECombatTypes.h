// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DECombatTypes.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECombatEventTrigger : uint8
{
    OnNone              UMETA(DisplayName = "없음"),
    OnHit               UMETA(DisplayName = "적 적중 시"),
    OnTakeDamage        UMETA(DisplayName = "피격 시"),
    OnKill              UMETA(DisplayName = "적 처치 시"),
    OnCrit              UMETA(DisplayName = "크리티컬 시"),
    OnSkillCast         UMETA(DisplayName = "스킬 시전 시"),
    OnPreHit            UMETA(DisplayName = "데미지 계산 직전")
};

// 2. 이벤트 발생 시 전달할 통합 데이터 (택배 상자)
USTRUCT(BlueprintType)
struct FCombatEventData
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "CombatEvent")
    AActor* Instigator; // 사건의 원인 제공자 (때린 놈)

    UPROPERTY(BlueprintReadWrite, Category = "CombatEvent")
    AActor* Target;     // 사건의 피해자 (맞은 놈)

    UPROPERTY(BlueprintReadWrite, Category = "CombatEvent")
    float DamageAmount; // 발생한 데미지량 (필요 시 사용)

    // ★ 추가됨: OnPreHit에서 이펙트들이 건드릴 수 있는 '데미지 배수'
    // 기본값은 1.0 (100%). 이펙트가 이걸 2.0으로 바꾸면 2배가 됨!
    UPROPERTY(BlueprintReadWrite)
    float DamageMultiplier = 1.0f;

    UPROPERTY(BlueprintReadWrite, Category = "CombatEvent")
    UObject* SourceObject;

    // 기본 생성자
    FCombatEventData() : Instigator(nullptr), Target(nullptr), DamageAmount(0.0f), SourceObject(nullptr) {}
};