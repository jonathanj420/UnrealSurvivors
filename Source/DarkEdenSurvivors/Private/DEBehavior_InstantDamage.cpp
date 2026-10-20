// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_InstantDamage.h"
#include "DESkillContext.h"
#include "DEHealthComponent.h"
#include "DEDamageTypes.h"
#include "DEAutoSkillBase.h"
#include "DEGameplayLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UDEBehavior_InstantDamage::Execute(FDESkillContext& Context)
{
    // 이전 시전의 남은 타격은 매니저가 기다려 주지만, 직접 Activate된 경우를 위해 정리한다
    EndBehavior();

    const int32 TotalHits = FMath::Max(1, HitCount);

    // 죽은 타겟을 빼는 건 이 Behavior 안에서만. Context.Targets는 뒤의 Behavior(넉백, 이펙트)가 그대로 쓴다
    TArray<AActor*> HitTargets = Context.Targets;

    if (HitInterval <= 0.f)
    {
        for (int32 i = 0; i < TotalHits && HitTargets.Num() > 0; ++i)
        {
            ApplyHit(Context, HitTargets);
        }
        return;
    }

    ApplyHit(Context, HitTargets);

    if (TotalHits > 1 && Context.Instigator && HitTargets.Num() > 0)
    {
        PendingContext = Context;
        PendingContext.Targets = MoveTemp(HitTargets);
        RemainingHits = TotalHits - 1;
        Context.Instigator->GetWorldTimerManager().SetTimer(
            HitTimerHandle, this, &UDEBehavior_InstantDamage::ApplyNextHit, HitInterval, true);
    }
}

bool UDEBehavior_InstantDamage::IsInProgress() const
{
    return PendingContext.Instigator && PendingContext.Instigator->GetWorldTimerManager().IsTimerActive(HitTimerHandle);
}

void UDEBehavior_InstantDamage::EndBehavior()
{
    if (PendingContext.Instigator)
    {
        PendingContext.Instigator->GetWorldTimerManager().ClearTimer(HitTimerHandle);
    }
    PendingContext = FDESkillContext();
    RemainingHits = 0;
}

void UDEBehavior_InstantDamage::ApplyNextHit()
{
    if (!PendingContext.Instigator)
    {
        EndBehavior();
        return;
    }

    ApplyHit(PendingContext, PendingContext.Targets);

    if (--RemainingHits <= 0 || PendingContext.Targets.Num() == 0)
    {
        EndBehavior();
    }
}

void UDEBehavior_InstantDamage::ApplyHit(const FDESkillContext& Context, TArray<AActor*>& Targets)
{
    // 1. 기본 데미지 계산
    float BaseDamage = Context.Damage;
    float FinalDamage = BaseDamage * DamageMultiplier; // 배율 적용

    // 2. 타겟 목록 순회. 죽은 타겟은 빼서, 간격 타격 중 풀에서 재사용된 몬스터를 때리지 않게 한다.
    // 앞에서부터 돌아야 OnHit 순서가 예전과 같다
    for (int32 TargetIndex = 0; TargetIndex < Targets.Num(); )
    {
        AActor* Target = Targets[TargetIndex];
        if (!Target || Target->IsPendingKillPending())
        {
            Targets.RemoveAt(TargetIndex);
            continue;
        }

        // 3. 체력 컴포넌트 찾기. 타겟은 몬스터 쿼리로만 들어오므로 없으면 대상이 아니다
        UDEHealthComponent* TargetHealth = Target->FindComponentByClass<UDEHealthComponent>();
        if (!TargetHealth || TargetHealth->IsDead())
        {
            Targets.RemoveAt(TargetIndex);
            continue;
        }
        ++TargetIndex;

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

        // 5. [로컬 OnPreHit] 조작 전용 첫 번째 이벤트 데이터
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
                    // 조건 만족 시 PreHitData.DamageMultiplier를 조작함
                    Effect->ExecuteEffect(PreHitData);
                }
            }
        }

        // 로컬 이펙트가 조작한 배수를 실제 청구서에 적용
        Req.BaseDamage *= PreHitData.DamageMultiplier;

        // 6. 라이브러리에 던지기 (글로벌 OnPreHit 개입 및 실제 데미지 연산)
        FDEDamageResult Res = UDEGameplayLibrary::ApplyCombatDamage(Req);

        // 이번 타격에 죽었으면 바로 뺀다. 다음 간격 타격 전에 풀에서 재사용되면 IsDead로는 걸러지지 않는다
        if (Res.bIsDead)
        {
            --TargetIndex;
            Targets.RemoveAt(TargetIndex);
        }

        // 무적 등으로 데미지가 0이 들어갔다면 아래 적중 효과는 생략
        if (Res.FinalDamage <= 0.0f) continue;

        // 7. [로컬 OnHit / OnKill] 결과 보고 전용 두 번째 이벤트 데이터
        FCombatEventData PostHitData;
        PostHitData.Instigator = Context.Instigator;
        PostHitData.Target = Target;
        PostHitData.DamageAmount = Res.FinalDamage; // 실제 들어간 데미지 기록
        PostHitData.SourceObject = Context.SourceSkill; // 로컬 이펙트가 후속 데미지(처형 등)를 낼 때 출처로 쓴다
        PostHitData.SourceContext = &Context;

        if (Context.SourceSkill)
        {
            for (UDECombatEffect* Effect : Context.SourceSkill->LocalEffects)
            {
                if (!Effect) continue;

                // 이 스킬 전용 출혈, 처형 등의 기믹
                if (Effect->TriggerCondition == ECombatEventTrigger::OnHit)
                {
                    Effect->ExecuteEffect(PostHitData);
                }
                else if (Effect->TriggerCondition == ECombatEventTrigger::OnKill && Res.bIsDead)
                {
                    Effect->ExecuteEffect(PostHitData);
                }
            }
        }
    }
}
