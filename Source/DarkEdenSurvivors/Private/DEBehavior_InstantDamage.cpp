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
            Req.DamageCauser = Context.Instigator;      // 스킬 주인 (혹은 투사체)
            Req.SourceObject = Context.ActiveSkill;     // skill itself
            Req.Victim = Target;
            // Context에 있는 정보 활용 (스킬마다 크확/크뎀이 다를 수 있음)
            Req.BaseDamage = FinalDamage;
            Req.CritChance = Context.CritChance;             // 스킬 컨텍스트에 크리티컬 확률이 있다고 가정
            Req.CritDamageMultiplier = Context.CritDamageMultiplier; // 스킬 컨텍스트에 크리티컬 배율이 있다고 가정

            // 라이브러리에 던지기 (피흡, 넉백, 킬 처리가 한 방에 끝남)
            //UE_LOG(LogTemp, Log, TEXT("Try DEGameplayLibrary"));
            FDEDamageResult Res = UDEGameplayLibrary::ApplyCombatDamage(Req, Context.FinalSnapshot, FVector::ZeroVector, 0.0f);
        }
        else
        {
            // 체력 컴포넌트가 없는 파괴 가능 오브젝트라면?
            // 기존 ApplyDamage를 안전망으로 사용 (선택)
            // UGameplayStatics::ApplyDamage(Target, FinalDamage, ...);
        }
    }

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
