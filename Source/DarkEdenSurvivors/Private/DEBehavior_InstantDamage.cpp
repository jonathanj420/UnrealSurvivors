// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_InstantDamage.h"
#include "DESkillContext.h"
#include "DEHealthComponent.h"
#include "DEDamageTypes.h"
#include "DEAutoSkillBase.h"
#include "DEGameplayLibrary.h"
#include "Kismet/GameplayStatics.h"

void UDEBehavior_InstantDamage::Execute(FDESkillContext& Context)
{

    // 1. 기본 데미지 계산
    float BaseDamage = Context.Damage;
    float FinalDamage = BaseDamage * DamageMultiplier; // 배율 적용

    // 2. 타겟 목록 순회
    for (AActor* Target : Context.Targets)
    {
        if (!Target || Target->IsPendingKillPending()) continue;

        // 3. 체력 컴포넌트 찾기 (캐싱)
        UDEHealthComponent* TargetHealth = Target->FindComponentByClass<UDEHealthComponent>();

        if (TargetHealth)
        {
            // 4. [AAA 스타일] 데미지 요청서(Request) 작성
            FDEDamageRequest Req;
            Req.Instigator = Context.Instigator;        // 시전한 사람
            Req.DamageCauser = Context.Instigator;      // 가해자
            Req.SourceObject = Context.SourceSkill;     // 스킬 본체
            Req.Victim = Target;
            Req.BaseDamage = FinalDamage;
            Req.CritChance = Context.CritChance;
            Req.CritDamageMultiplier = Context.CritDamageMultiplier;
            Req.DamageTags = SkillDamageTags;

            // =========================================================
            // ★ 5. [로컬 OnPreHit] 조작 전용 '첫 번째' 깨끗한 택배 상자
            // =========================================================
            FCombatEventData PreHitData;
            PreHitData.Instigator = Context.Instigator;
            PreHitData.Target = Target;
            PreHitData.DamageMultiplier = 1.0f; // 오염되지 않은 기본 1배수
            PreHitData.SourceContext = &Context;

            if (Context.SourceSkill)
            {
                // 스킬 주머니(LocalEffects)에 있는 OnPreHit 이펙트들을 실행
                for (UDECombatEffect* Effect : Context.SourceSkill->LocalEffects)
                {
                    if (Effect && Effect->TriggerCondition == ECombatEventTrigger::OnPreHit)
                    {
                        // 조건 만족 시 PreHitData.DamageMultiplier를 조작함!
                        Effect->ExecuteEffect(PreHitData);
                    }
                }
            }

            // 로컬 이펙트가 조작한 배수를 실제 청구서에 적용!
            Req.BaseDamage *= PreHitData.DamageMultiplier;

            // =========================================================
            // ★ 6. 라이브러리에 던지기 (글로벌 OnPreHit 개입 및 실제 데미지 연산)
            // =========================================================
            // 이 안에서 플레이어의 패시브가 알아서 싹 다 터짐
           // UE_LOG(LogTemp, Error, TEXT("Try to Damage in InstantDamage Behavior"));
            FDEDamageResult Res = UDEGameplayLibrary::ApplyCombatDamage(Req);
            //UE_LOG(LogTemp, Error, TEXT("Applied Final Damage in InstantDamage Behavior"));
            // 무적 등으로 데미지가 0이 들어갔다면 아래 적중 효과는 쿨하게 생략
            if (Res.FinalDamage <= 0.0f) continue;

            // =========================================================
            // ★ 7. [로컬 OnHit / OnKill] 결과 보고 전용 '두 번째' 깨끗한 택배 상자
            // =========================================================
            FCombatEventData PostHitData;
            PostHitData.Instigator = Context.Instigator;
            PostHitData.Target = Target;
            PostHitData.DamageAmount = Res.FinalDamage; // 실제 들어간 찐 데미지 기록
            PostHitData.SourceContext = &Context;
            // PostHitData.DamageMultiplier는 건드리지 않았으니 안전한 1.0 상태!

            if (Res.bIsDead)
            {
               // UE_LOG(LogTemp, Error, TEXT("Completely Dead . . . "));
            }
            else
            {
               //UE_LOG(LogTemp, Error, TEXT("WTF?"));
            }
            if (Context.SourceSkill)
            {
                for (UDECombatEffect* Effect : Context.SourceSkill->LocalEffects)
                {
                    if (!Effect) continue;

                    // 이 스킬 전용 출혈, 처형 등의 기믹이 여기서 안전하게 터짐!
                    if (Effect->TriggerCondition == ECombatEventTrigger::OnHit)
                    {
                        Effect->ExecuteEffect(PostHitData);
                        //UE_LOG(LogTemp, Error, TEXT("Applied OnHit Effects in InstantDamage Behavior"));
                    }
                    else if (Effect->TriggerCondition == ECombatEventTrigger::OnKill && Res.bIsDead)
                    {
                        Effect->ExecuteEffect(PostHitData);
                       // UE_LOG(LogTemp, Error, TEXT("Applied OnKill Effects in InstantDamage Behavior"));
                    }
                }
                
            }
           // UE_LOG(LogTemp, Error, TEXT("Tried Effects In InstantDamage Behavior"));
        }
        else
        {
            // 체력 컴포넌트가 없는 파괴 가능 오브젝트(항아리, 나무 상자 등)라면 기존 엔진 함수로 처리
            UGameplayStatics::ApplyDamage(Target, FinalDamage, Context.Instigator->GetInstigatorController(), Context.Instigator, nullptr);
        }
    }
    //// 1. 기본 데미지 계산
    //float BaseDamage = Context.Damage;
    //float FinalDamage = BaseDamage * DamageMultiplier; // 배율 적용

    //// 2. 타겟 목록 순회
    //for (AActor* Target : Context.Targets)
    //{
    //    if (!Target || Target->IsPendingKillPending()) continue;

    //    // 3. 체력 컴포넌트 찾기 (캐싱)
    //    UDEHealthComponent* TargetHealth = Target->FindComponentByClass<UDEHealthComponent>();

    //    if (TargetHealth)
    //    {
    //        // 4. [AAA 스타일] 데미지 요청서(Request) 작성
    //        FDEDamageRequest Req;
    //        Req.Instigator = Context.Instigator;        // 시전한 사람
    //        Req.DamageCauser = Context.Instigator;      // 스킬 주인 (혹은 투사체)
    //        Req.SourceObject = Context.ActiveSkill;     // skill itself
    //        Req.Victim = Target;
    //        // Context에 있는 정보 활용 (스킬마다 크확/크뎀이 다를 수 있음)
    //        Req.BaseDamage = FinalDamage;
    //        Req.CritChance = Context.CritChance;             // 스킬 컨텍스트에 크리티컬 확률이 있다고 가정
    //        Req.CritDamageMultiplier = Context.CritDamageMultiplier; // 스킬 컨텍스트에 크리티컬 배율이 있다고 가정

    //        // 라이브러리에 던지기 (피흡, 넉백, 킬 처리가 한 방에 끝남)
    //        //UE_LOG(LogTemp, Log, TEXT("Try DEGameplayLibrary"));
    //        FDEDamageResult Res = UDEGameplayLibrary::ApplyCombatDamage(Req, Context.FinalSnapshot, FVector::ZeroVector, 0.0f);
    //    }
    //    else
    //    {
    //        // 체력 컴포넌트가 없는 파괴 가능 오브젝트라면?
    //        // 기존 ApplyDamage를 안전망으로 사용 (선택)
    //        // UGameplayStatics::ApplyDamage(Target, FinalDamage, ...);
    //    }
    //}

    //// Context에서 기본 데미지 가져오기
    //float BaseDamage = Context.Damage;
    //float FinalDamage = BaseDamage * DamageMultiplier;

    //// 타겟 목록 순회하며 데미지
    //for (AActor* Target : Context.Targets)
    //{
    //	if (Target && !Target->IsPendingKillPending())
    //	{
    //		UGameplayStatics::ApplyDamage(
    //			Target,
    //			FinalDamage,
    //			Context.Instigator->GetInstigatorController(),
    //			Context.Instigator,
    //			UDamageType::StaticClass()
    //		);
    //	}
    //}

}
