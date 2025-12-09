// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBloodyNail.h"
#include "DEMonsterBase.h"
#include "Engine/EngineTypes.h" 
#include "DrawDebugHelpers.h"
#include "DEMonsterSpawnManager.h"
ADEBloodyNail::ADEBloodyNail()
{
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NS_Niagara(TEXT("/Game/DarkEden/Data/Niagara/NS_BloodyNail.NS_BloodyNail"));
	if (NS_Niagara.Succeeded())
	{
		NiagaraSystem = NS_Niagara.Object;
	}
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	NiagaraEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraEffect"));
	NiagaraEffectComponent->SetupAttachment(RootComponent);
	NiagaraEffectComponent->bAutoActivate = false; // 기본 비활성화

	// 기본 값
	Damage = 50.f;
	KnockbackForce = 500.f;
	SweepRadius = 200.f;
	CritChance = 0.1f;
	bCanCrit = true;
	bIsCrit = false;
	BaseRadius = 60.0f;
}

void ADEBloodyNail::PerformSweepAttack()
{
	Super::PerformSweepAttack();
    //DoFanShapeAttack(Damage, 200.0f, 90);

}

void ADEBloodyNail::DoFanShapeAttack(float inDamage, float Range, float AngleDegrees)
{
    Super::ResetState();
    if (!GetOwner()) return;

    FVector SweepOrigin = GetActorLocation();
    FVector ForwardDir = GetActorForwardVector().GetSafeNormal();

    float HalfAngle = 90 * 0.5f; // degrees
    float Radius = 200.0f;

    // 부채꼴 디버그 그리기
    const int32 NumLines = 16; // 부채꼴 선 개수
    for (int32 i = 0; i <= NumLines; ++i)
    {
        float Alpha = -HalfAngle + (90.0f * i / NumLines); // -45 ~ 45 같은 범위
        FRotator Rot(0.f, Alpha, 0.f);
        FVector Dir = Rot.RotateVector(ForwardDir);
        DrawDebugLine(GetWorld(), SweepOrigin, SweepOrigin + Dir * Radius, FColor::Red, false, 1.f, 0, 2.f);
    }

    // 몬스터 범위 검사
    for (ADEMonsterBase* Mob : MonsterManager->ActiveMonsters)
    {
        if (!Mob) continue;

        FVector ToMob = Mob->GetActorLocation() - SweepOrigin;
        float Dist = ToMob.Size();

        if (Dist > Radius) continue;

        FVector ToMobDir = ToMob / Dist; // 정규화
        float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardDir, ToMobDir)));

        if (AngleDeg <= HalfAngle)
        {
            // 범위 안에 있는 몬스터
            UGameplayStatics::ApplyDamage(Mob, Damage, nullptr, this, nullptr);

            // 넉백
            Mob->ApplyKnockback(ToMobDir, KnockbackForce);

            // 히트 디버그
            DrawDebugSphere(GetWorld(), Mob->GetActorLocation(), 30.f, 12, FColor::Green, false, 1.f);
        }
    }

    // 이펙트
    if (NiagaraSystem)
    {
        float ScaleFactor = Radius / BaseRadius;
        NiagaraEffectComponent->SetWorldScale3D(FVector(ScaleFactor));
        NiagaraEffectComponent->SetAsset(NiagaraSystem);
        NiagaraEffectComponent->Activate(true);
    }
}

void ADEBloodyNail::BeginPlay()
{
    Super::BeginPlay();
    if (!MonsterManager)
    {
        MonsterManager = Cast<ADEMonsterSpawnManager>(
            UGameplayStatics::GetActorOfClass(GetWorld(), ADEMonsterSpawnManager::StaticClass())
        );
    }
}
