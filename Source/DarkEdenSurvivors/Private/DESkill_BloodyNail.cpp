// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_BloodyNail.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "DEBehavior_CullByCone.h"
#include "DEBehavior_InstantDamage.h"
#include "DEBehavior_PlayEffect.h"
#include "DEBehavior_ApplyKnockback.h"



UDESkill_BloodyNail::UDESkill_BloodyNail()
{
    // 1. 생성자에서 에셋 경로를 하드코딩해서 찾아옴 (우클릭 -> 레퍼런스 복사)
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FXAsset(TEXT("/Game/DarkEden/Data/Niagara/NS_BloodyNail.NS_BloodyNail"));
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

void UDESkill_BloodyNail::InitBehaviors()
{
    Super::InitBehaviors();

    if (Behaviors.Num() > 0)
    {
        return;
    }

    // ==========================================
    // 블러디 네일 비헤이비어 파이프라인 조립
    // ==========================================

    // 1. 타겟 수집 & 필터링 (통합된 부채꼴 판정 한 방에 처리!)
    UDEBehavior_CullByCone* CullCone = NewObject<UDEBehavior_CullByCone>(this);
    //CullCone->bShowDebug = true;     // ★ 에디터에서 빨간색 피자 조각 보이게 켜둠!
    Behaviors.Add(CullCone);

    UDEBehavior_PlayEffect* PlayFX = NewObject<UDEBehavior_PlayEffect>(this);
    PlayFX->NiagaraTargetType = EEffectTargetType::Instigator;
    PlayFX->NiagaraEffect = NailHitEffect; // ★ 여기에 쏙!
    PlayFX->RotationType = EEffectRotation::InstigatorForward;
    PlayFX->SoundEffect = NailHitSound;    // ★ 여기에 쏙!
    PlayFX->SizeVariableName = FName("SkillRadius");
    Behaviors.Add(PlayFX);

    // 3. 즉발 데미지 적용
    UDEBehavior_InstantDamage* InstantDmg = NewObject<UDEBehavior_InstantDamage>(this);
    Behaviors.Add(InstantDmg);

    UDEBehavior_ApplyKnockback* Knockback = NewObject< UDEBehavior_ApplyKnockback>(this);
    Behaviors.Add(Knockback);
}
