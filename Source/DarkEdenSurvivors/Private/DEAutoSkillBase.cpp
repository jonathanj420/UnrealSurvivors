// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAutoSkillBase.h"
#include "DECombatComponent.h"
#include "DESkillBehavior.h"
#include "DESimpleAOEBase.h"

void UDEAutoSkillBase::Activate()
{
	if (!SkillOwner) return;
    //UE_LOG(LogTemp, Warning, TEXT("%s Activated"), *GetNameSafe(this));
	//FDESkillContext Context;

    //zis no = game boom
    CachedContext = FDESkillContext();


	BuildContext(CachedContext);

    // =========================================================
    // ★ [핵심 1] 지속형 스킬(성서)이라면 '시전 중' 상태로 돌입!
    // =========================================================
    if (bCooldownAfterDuration && CachedContext.Duration > 0.0f)
    {
        bIsRunning = true; // 이걸 true로 하면 매니저가 쿨타임을 안 깎고 기다림

        // 지속시간(Duration) 뒤에 FinishSkill을 실행하는 타이머 작동!
        SkillOwner->GetWorldTimerManager().SetTimer(DurationTimerHandle, this, &UDEAutoSkillBase::FinishSkill, CachedContext.Duration, false);
    }

    // Context를 만들었으니, 실행 로직으로 던져줍니다.
    ExecuteWithContext(CachedContext);
}

void UDEAutoSkillBase::ExecuteWithContext(FDESkillContext& Context)
{

    //here could be other skills' own logics



    // 기본 역할: Behavior들 실행
    for (UDESkillBehavior* Behavior : Behaviors)
    {
        if (Behavior) Behavior->Execute(Context);
    }



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
	Behaviors.Empty();
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
    bIsRunning = false; // 이제 끝났으니 매니저한테 "쿨타임 돌려라!" 라고 허락함

    // 네가 기존에 잘 만들어둔 AOE 정리(해골 지우기) 함수를 재활용!
    EndSkill();
}

void UDEAutoSkillBase::EndSkill()
{
    // AOE 정리
    for (auto& Pair : OwnedAOEMap)
    {
        if (Pair.Value.IsValid())
            Pair.Value->ReturnToPool();
    }
    OwnedAOEMap.Reset();
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
