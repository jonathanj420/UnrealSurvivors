// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_TalonOfCrow.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DEBehavior_CullByCone.h"
#include "DEBehavior_InstantDamage.h"
#include "DEBehavior_PlayEffect.h"
#include "DEBehavior_ApplyKnockback.h"
#include "DEGameplayLibrary.h"
#include "DECombatEffect_CooldownReduction.h"


UDESkill_TalonOfCrow::UDESkill_TalonOfCrow()
{
    // 1. 생성자에서 에셋 경로를 하드코딩해서 찾아옴 (우클릭 -> 레퍼런스 복사)
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FXAsset(TEXT("/Game/DarkEden/Data/Niagara/NS_TalonOfCrow.NS_TalonOfCrow"));
    if (FXAsset.Succeeded())
    {
        NailHitEffect = FXAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Vampire_attack.Vampire_attack"));
    if (SoundAsset.Succeeded())
    {
        NailHitSound = SoundAsset.Object;
    }

}

void UDESkill_TalonOfCrow::InitBehaviors()
{
    Super::InitBehaviors();
    

    UDEBehavior_PlayEffect* PlayFX = NewObject<UDEBehavior_PlayEffect>(this);
    PlayFX->TargetType = EEffectTargetType::Instigator;
    PlayFX->NiagaraEffect = NailHitEffect; // ★ 여기에 쏙!
    PlayFX->SoundEffect = NailHitSound;    // ★ 여기에 쏙!
    PlayFX->RotationType = EEffectRotation::TowardTarget;
    Behaviors.Add(PlayFX);

    UDEBehavior_CullByCone* CullCone = NewObject<UDEBehavior_CullByCone>(this);
    //CullCone->Radius = 150.0f;       // 손톱 사거리
    //CullCone->ConeAngle = 120.0f;    // 넓게 할퀴는 부채꼴 각도
    ////CullCone->bShowDebug = true;     // ★ 에디터에서 빨간색 피자 조각 보이게 켜둠!
    Behaviors.Add(CullCone);

    // 3. 즉발 데미지 적용
    UDEBehavior_InstantDamage* InstantDmg = NewObject<UDEBehavior_InstantDamage>(this);
    Behaviors.Add(InstantDmg);

    UDEBehavior_ApplyKnockback* Knockback = NewObject< UDEBehavior_ApplyKnockback>(this);
    Behaviors.Add(Knockback);

    UDECombatEffect_CooldownReduction* CooldownResetEffect = NewObject<UDECombatEffect_CooldownReduction>(this);
    CooldownResetEffect->TriggerCondition = ECombatEventTrigger::OnKill;

    // ★ 핵심: 하드코딩 10 대신, 블루프린트에서 세팅한 내 ID를 동적으로 가져옴!
    CooldownResetEffect->SpecificSkillID = this->SkillID;
    CooldownResetEffect->Amount = 0.5f;
    CooldownResetEffect->InternalCooldown = 0.1f;
    UE_LOG(LogTemp, Warning, TEXT("WTF IS TALON OF CROW SKILL ID : %d"), this->SkillID);
    CooldownResetEffect->TargetSkill = ECDRTargetSkill::SpecificSkill;
    //CooldownResetEffect->bInstantReset = true;

    LocalEffects.Add(CooldownResetEffect);
}

void UDESkill_TalonOfCrow::ExecuteWithContext(FDESkillContext& Context)
{
    for (int32 i = 0; i < Context.Amount; i++)
    {
        AActor* Target = nullptr;

        if (i == 0)
            Target = UDEGameplayLibrary::GetNearestTarget(SkillOwner, -1.0f);
        else
            Target = UDEGameplayLibrary::GetRandomTargets(SkillOwner, -1.0f, 1)[0];

        if (!Target) continue;

        Context.Targets.Empty();
        Context.Targets.Add(Target);
        Super::ExecuteWithContext(Context);
    }

   
}
