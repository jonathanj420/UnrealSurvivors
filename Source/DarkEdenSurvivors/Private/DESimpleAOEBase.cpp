// Fill out your copyright notice in the Description page of Project Settings.


#include "DESimpleAOEBase.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DEPoolSubsystem.h"
#include "DESkillContext.h"
#include "DEDamageTypes.h"
#include "DEGameplayLibrary.h"
#include "DEAutoSkillBase.h"
#include "DEHealthComponent.h"
#include "Engine/World.h"
#include "DEMonsterQuerySubsystem.h"
#include "DEMonsterBase.h"

ADESimpleAOEBase::ADESimpleAOEBase()
{
    // [중요] 지속 데미지(Tick)를 줘야 하므로 Tick을 켜야 합니다.
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent->SetCollisionProfileName(TEXT("PlayerAttack"));
}

void ADESimpleAOEBase::BeginPlay()
{
    Super::BeginPlay();

    // 동적 델리게이트 바인딩 (들어오자마자 때리기 위해)
    
}


void ADESimpleAOEBase::InitializeFromContext(const FDESkillContext& Context)
{
    // 2. 장판(AOE) 전용 데이터 세팅
    HitCooldown = Context.GetValue(TEXT("HitInterval"), 0.5f);
    //UE_LOG(LogTemp, Warning, TEXT("%s Try Init from ContXXt"), *GetName());
    // 1. 공통 스탯(데미지, 범위, 오너, 수명, 풀링 리셋 등)은 부모에게 짬처리!
    Super::InitializeFromContext(Context);

   

    // 3. 영구 지속 장판 처리 (독 장판, 마늘 등)
    if (LifeTime < 0.0f)
    {
        bInfiniteDuration = true;
        // 부모가 켜둔 수명 타이머를 장판은 강제로 끕니다 (영원히 안 죽음)
        GetWorldTimerManager().ClearTimer(LifeTimeTimerHandle);
    }
    else
    {
        bInfiniteDuration = false;
    }

    // 4. (선택) 나이아가라 크기 자동 조절 연동
    if (NiagaraComponent)
    {
        // 부모가 세팅해둔 Size(또는 Context.Radius) 활용
        NiagaraComponent->SetFloatParameter(FName("SkillRadius"), Context.Radius);
    }

    CollisionComponent->SetSphereRadius(Radius);


}

void ADESimpleAOEBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bHitOnContact)
    {
        PulseTimer -= DeltaTime;
        if (PulseTimer > 0.0f) return;
        PulseTimer = HitCooldown;
    }

    const UDEMonsterQuerySubsystem* Query = GetWorld()->GetSubsystem<UDEMonsterQuerySubsystem>();
    if (!Query) return;

    TArray<AActor*> Targets;
    Query->QueryCircle(GetInstigator(), GetActorLocation(), Radius, Targets);

    for (AActor* Target : Targets)
    {
        if (bHitOnContact && !CanHitTarget(Target)) continue;
        OnHitTarget(Target);
    }
}

bool ADESimpleAOEBase::CanHitTarget(AActor* Target) const
{
    if (!Target) return false;

    // 기록이 없으면(처음 만남) 즉시 타격 가능
    const float* NextHitTime = HitCooldownMap.Find(Target);
    return !NextHitTime || GetWorld()->GetTimeSeconds() >= *NextHitTime;
}

void ADESimpleAOEBase::OnHitTarget(AActor* Target)
{
    HitCooldownMap.Add(Target, GetWorld()->GetTimeSeconds() + HitCooldown);
    TryDealDamage(Target);
}

void ADESimpleAOEBase::ResetState()
{
    
    Super::ResetState();

    // 맵 초기화
    
}

void ADESimpleAOEBase::ReturnToPool()
{
    Super::ReturnToPool();
    PulseTimer = 0.0f;
}
