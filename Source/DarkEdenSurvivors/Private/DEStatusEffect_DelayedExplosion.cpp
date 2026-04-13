// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffect_DelayedExplosion.h"
#include "DEGameplayLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/World.h"

void UDEStatusEffect_DelayedExplosion::ExecuteAction(AActor* Target, FActiveStatusEffect& EffectData) const
{
    UE_LOG(LogTemp, Warning, TEXT("Try Delayed Explosion : %s"), *GetName());
    UWorld* World = Target->GetWorld();
    if (!World) return;

    FVector ExplosionCenter = Target->GetActorLocation();

    UNiagaraComponent* SpawnedComp = nullptr;

    // 1. 파티클 펑!
    if (ExplosionParticle)
        SpawnedComp=UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, ExplosionParticle, ExplosionCenter);
    
    

    // 2. 광역 데미지 전송용 택배 상자(Request) 조립
    FDEDamageRequest Req;
    Req.Instigator = EffectData.Instigator.Get();
    Req.DamageCauser = EffectData.Instigator.Get();
    Req.SourceObject = const_cast<UDEStatusEffect_DelayedExplosion*>(this);
    Req.BaseDamage = EffectData.Power;
    Req.DamageTags = StatusTags;

    float FinalRadius = (EffectData.SourceContext.Radius > 0.0f) ? EffectData.SourceContext.Radius : ExplosionRadius;

    if (SpawnedComp)
    {
        SpawnedComp->SetVariableFloat("SkillRadius", FinalRadius);
    }

    // 3. 예외 대상 (플레이어) 지정
    TArray<AActor*> Ignored;
    if (Req.Instigator) Ignored.Add(Req.Instigator);

    // ★ 4. 라이브러리 함수 단 한 줄 호출로 끝! ★
    UDEGameplayLibrary::ApplyAoEDamage(World, ExplosionCenter, FinalRadius, Req, Ignored);
}