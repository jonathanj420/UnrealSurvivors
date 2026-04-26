// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAutoSkillBase.h"
#include "DECombatComponent.h"
#include "DESkillBehavior.h"
#include "DESkillActorBase.h"

void UDEAutoSkillBase::Activate()
{
	if (!SkillOwner) return;
    UE_LOG(LogTemp, Warning, TEXT("%s Activated"), *GetNameSafe(this));
	//FDESkillContext Context;

    //zis no = game boom
    CachedContext = FDESkillContext();


	BuildContext(CachedContext);

    if (!SkillData) return;

    switch (SkillData->ExecutionType)
    {
    case ESkillExecutionType::Instant:
        // 아무것도 안 함, 발동 후 바로 쿨타임
        break;

    case ESkillExecutionType::Duration:
        if (CachedContext.Duration > 0.0f)
        {
            bIsRunning = true;
            SkillOwner->GetWorldTimerManager().SetTimer(
                DurationTimerHandle, this,
                &UDEAutoSkillBase::FinishSkill,
                CachedContext.Duration, false);
        }
        break;

    case ESkillExecutionType::Permanent:
        if (bIsRunning) return; // 중복 발동 방지
        bIsRunning = true;
        // 타이머 없음, FinishSkill 호출 안 됨
        break;
    }

    ExecuteWithContext(CachedContext);
    //// =========================================================
    //// ★ [핵심 1] 지속형 스킬(성서)이라면 '시전 중' 상태로 돌입!
    //// =========================================================
    //if (bCooldownAfterDuration && CachedContext.Duration > 0.0f)
    //{
    //    bIsRunning = true; // 이걸 true로 하면 매니저가 쿨타임을 안 깎고 기다림

    //    // 지속시간(Duration) 뒤에 FinishSkill을 실행하는 타이머 작동!
    //    SkillOwner->GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &UDEAutoSkillBase::FinishSkill, CachedContext.Duration, false);
    //}

    //// Context를 만들었으니, 실행 로직으로 던져줍니다.
    //ExecuteWithContext(CachedContext);


 

}

void UDEAutoSkillBase::ExecuteWithContext(FDESkillContext& Context)
{

    for (UDESkillBehavior* Behavior : Behaviors)
    {
        if (Behavior) Behavior->Execute(Context);
    }



    //here could be other skills' own logics



    // 기본 역할: Behavior들 실행
  /*  for (UDESkillBehavior* Behavior : Behaviors)
    {
        if (Behavior) Behavior->Execute(Context);
    }
*/


    //or maybe here

}

void UDEAutoSkillBase::SetSkillData(const FDESkillData* NewData)
{
	SkillData = NewData;
	if (SkillData) CurrentLevel = SkillData->Level;
	UE_LOG(LogTemp, Warning, TEXT("[Skill] %s : level : %d, "), *NewData->SkillName,NewData->Level);
}

void UDEAutoSkillBase::InitBehaviors()
{
	//Behaviors.Empty();
}

void UDEAutoSkillBase::BuildContext(FDESkillContext& OutContext)
{
    // 1. 필수 참조 연결
    OutContext.Instigator = SkillOwner;
    OutContext.SourceSkill = this;
    // 데이터가 없으면 중단
    if (!SkillData) return;

    //// 2. 컴포넌트 가져오기 (캐싱)
    //UDECombatComponent* Combat = nullptr;
    //if (SkillOwner)
    //{
    //    Combat = SkillOwner->FindComponentByClass<UDECombatComponent>();
    //}

    // =========================================================
    // [3. 초기값 설정: 데이터테이블(Raw Data)]
    // =========================================================
    float FinalDamage = SkillData->Damage;
    int32 FinalAmount = SkillData->Amount;
    float FinalSpeed = SkillData->Speed;
    float FinalDuration = SkillData->Duration;
    float FinalRadius = SkillData->Radius;
    float FinalKnockback = SkillData->KnockbackForce;
    int32 FinalPenetration = SkillData->Penetration;

    // 치명타 관련 (기본값 설정)
    float FinalCritChance = SkillData->CritChance;
    float FinalCritDmgMultiplier = 2.0f; // 안전장치: 컴포넌트 없을 때의 기본 배율

    // =========================================================
    // [4. 컴포넌트 보정값 적용 (Modifiers via Snapshot)]
    // =========================================================
    if (CachedCombatComp)
    {
        // ★ 핵심: CombatComponent에서 정제된 스냅샷을 가져옵니다.
        // (쿨타임 캡, 소수점 처리 등이 이미 완료된 상태)
        FCombatSnapshot PlayerStat = CachedCombatComp->GetCombatSnapshot();
        OutContext.FinalSnapshot = PlayerStat;
        // 1. 데미지 공식: (기본뎀) * 데미지배율
        // ※ 추후 '고정 공격력(Atk)'이 생긴다면: (FinalDamage + PlayerStat.AttackPower) * PlayerStat.FinalDamageMultiplier;
        FinalDamage = FinalDamage * PlayerStat.FinalDamageMultiplier;

        // 2. 투사체 개수: 기본 + 보너스 개수
        FinalAmount += PlayerStat.BonusAmount;

        // 3. 유틸리티 스탯 (속도, 지속시간, 범위 등): 기본 * 배율
        FinalSpeed *= PlayerStat.ProjectileSpeedMultiplier;
        FinalDuration *= PlayerStat.DurationMultiplier;
        FinalRadius *= PlayerStat.EffectSizeMultiplier;

        // (참고: 넉백이나 관통도 필요하다면 여기서 PlayerStat을 이용해 보정 가능)
        //so i did
        FinalKnockback *= PlayerStat.KnockbackMultiplier;

        // 4. 치명타 로직 (AAA 스타일 합산 방식)
        // -1.0f는 "치명타 절대 발동 안 함"을 의미 (도트딜, 장판 등)
        if (FinalCritChance >= 0.0f)
        {
            // [A] 확률: 단순 합연산 (5% + 10% = 15%)
            FinalCritChance += PlayerStat.CritChance;

            // [B] 데미지 배율: "아이템 보너스만 추출해서 합산"
            // 공식: 스킬고유배율 + (플레이어현재 - 플레이어기본(2.0))

            // 데이터테이블에 설정된 스킬 고유 배율 (예: 저격 = 5.0)
            float SkillBaseCrit = SkillData->CritDamageMultiplier; // FDESkillData에 이 변수가 있어야 함

            if (SkillBaseCrit > 0.0f)
            {
                // 특수 스킬(저격 등): 스킬 배율(5.0) + 아이템성장치(0.8) = 5.8
                // 2.0f는 시스템 기본 치명타 배율
                FinalCritDmgMultiplier = SkillBaseCrit + (PlayerStat.CritDamageMultiplier - 2.0f);
            }
            else
            {
                // 일반 스킬: 그냥 플레이어 스탯을 그대로 사용 (예: 2.8)
                FinalCritDmgMultiplier = PlayerStat.CritDamageMultiplier;
            }
        }
        else
        {
            // 치명타 불가 스킬
            FinalCritChance = 0.0f;
            FinalCritDmgMultiplier = 1.0f; // 배율 1.0 (노크리)
        }
    }

    // =========================================================
    // [5. 최종 결과 저장 (Final Snapshot)]
    // =========================================================

    // 이제 Context에는 "모든 계산이 끝난 최종값"이 들어갑니다.
    OutContext.Damage = FinalDamage;
    OutContext.Amount = FinalAmount;
    OutContext.Penetration = FinalPenetration;
    OutContext.Speed = FinalSpeed;
    OutContext.KnockbackForce = FinalKnockback;
    OutContext.Radius = FinalRadius;
    OutContext.Duration = FinalDuration;

    // [중요] 치명타 정보 스냅샷
    OutContext.CritChance = FinalCritChance;
    OutContext.CritDamageMultiplier = FinalCritDmgMultiplier;
    
    // 6. 맵 데이터(옵션) 통째로 복사
    // (특수 기믹을 위한 커스텀 데이터)
    OutContext.CustomValues = SkillData->OptionValues;
}


void UDEAutoSkillBase::FinishSkill()
{
    CurrentBehaviorIndex = 0;

    bIsRunning = false; // 매니저한테 "쿨타임 돌려라!" 라고 허락함

    EndSkill(); // 기존 AOE, 이펙트 풀링 정리 함수 재활용
}

void UDEAutoSkillBase::EndSkill()
{
    // AOE 정리
    for (auto& Pair : OwnedSkillActorMap)
    {
        if (Pair.Value.IsValid())
            Pair.Value->ReturnToPool();
    }
    OwnedSkillActorMap.Reset();

    for (UDESkillBehavior* Behavior : Behaviors)
    {
        if (Behavior)
        {
            Behavior->EndBehavior(); // 다형성에 의해 SpawnOnMove는 타이머를 끄게 됨
        }
    }
}

void UDEAutoSkillBase::OnTargetKilled(const FDEDamageResult& Result)
{
}

int32 UDEAutoSkillBase::GetBaseAmount() const
{
    // 스킬 데이터가 캐싱되어 있다면 그 값을 그대로 돌려줌
    if (SkillData)
    {
        return SkillData->Amount;
    }

    // 데이터가 세팅되지 않았을 경우의 안전장치
    return 1;
}

ESkillExecutionType UDEAutoSkillBase::GetExecutionType() const
{
    if (!SkillData) return ESkillExecutionType::Instant;
    return SkillData->ExecutionType;
}

void UDEAutoSkillBase::RefreshContext()
{
    BuildContext(CachedContext);
    for (UDESkillBehavior* Behavior : Behaviors)
    {
        if (Behavior) Behavior->OnContextRefreshed(CachedContext);
    }
}

void UDEAutoSkillBase::ExecutePipeline()
{
    // 배열 끝까지 돌 때까지 반복
    while (CurrentBehaviorIndex < Behaviors.Num())
    {
        UDESkillBehavior* CurrentBehavior = Behaviors[CurrentBehaviorIndex];
        CurrentBehaviorIndex++; // 다음 인덱스로 선 이동 (타이머 재개 시 여기서부터 시작)

        if (!CurrentBehavior) continue;

        // 1. 비헤이비어 본연의 역할 실행 (데미지, 사운드 등)
        CurrentBehavior->Execute(CachedContext);

        // 2. 파이프라인을 멈춰야 하는 딜레이가 있는지 확인 (다형성 활용)
        float PipelineDelay = CurrentBehavior->GetPipelineDelay();
        if (PipelineDelay > 0.0f)
        {
            if (SkillOwner)
            {
                // 3. 딜레이가 있다면, 타이머 세팅 후 while 루프를 즉시 빠져나감 (일시정지)
                SkillOwner->GetWorldTimerManager().SetTimer(
                    PipelineTimerHandle,
                    this,
                    &UDEAutoSkillBase::ExecutePipeline, // 시간이 되면 알아서 다시 들어와서 재개됨
                    PipelineDelay,
                    false
                );
            }
            return; // ★ 루프 강제 탈출 (매우 중요)
        }
    }

    // (참고) while 문을 모두 통과했다 = 배열 끝까지 다 돌았다.
    // 만약 파이프라인이 끝나는 시점에 스킬을 완전 종료하고 싶다면 여기서 처리할 수 있습니다.
    // 하지만 현재 유저분 구조는 Activate()의 ExecutionType 타이머가 FinishSkill을 부르므로 생략해도 무방합니다.
}

void UDEAutoSkillBase::CancelSkill()
{
    // ★ 허공에 스킬 나가는 '유령 스킬' 버그 완벽 차단
    if (SkillOwner)
    {
        SkillOwner->GetWorldTimerManager().ClearTimer(PipelineTimerHandle);
        SkillOwner->GetWorldTimerManager().ClearTimer(DurationTimerHandle); // 혹시 모를 지속시간 타이머도 해제
    }

    CurrentBehaviorIndex = 0;
    bIsRunning = false;

    EndSkill(); // 스폰된 이펙트나 장판 치우기
}
