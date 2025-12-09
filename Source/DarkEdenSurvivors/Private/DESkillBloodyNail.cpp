// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillBloodyNail.h"
#include "DEBloodyNail.h"
#include "Engine/World.h"
#include "DEPoolSubsystem.h"


UDESkillBloodyNail::UDESkillBloodyNail()
{
	AttackClass = ADEBloodyNail::StaticClass();
    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Vampire_attack.Vampire_attack")
    );

    if (SoundObj.Succeeded())
    {
        AttackSound = SoundObj.Object;
    }
}

void UDESkillBloodyNail::ActivateSkill(FDESkillData* SkillData)
{
    if (!SkillData || !SkillOwner || !AttackClass) return;

    UWorld* World = SkillOwner->GetWorld();
    if (!World) return;

    float EffectRoll = bRightSide ? 45.f : -45.f;
    bRightSide = !bRightSide; // 다음 공격에 반대로

    FRotator EffectRotation = FRotator(0.0f, 0.0f, EffectRoll);


    FVector SpawnLoc = SkillOwner->GetActorLocation();// +SkillOwner->GetActorForwardVector() * 200.f;
    FRotator SpawnRot = SkillOwner->GetActorRotation() + EffectRotation;

    UDEPoolSubsystem* Pool = SkillOwner->GetWorld()->GetGameInstance()->GetSubsystem<UDEPoolSubsystem>();
    if (!Pool) return;

    FVector Dir = SpawnRot.Vector();

    AActor* Pooled = Pool->GetPooledActor(
        AttackClass,
        SpawnLoc,
        SpawnRot,
        true
    );

    if (auto* MeleeActor = Cast<ADEBloodyNail>(Pooled))
    {
        //MeleeActor->SetActorRelativeRotation(SpawnRot);
        MeleeActor->SetOwner(SkillOwner);

        // 스킬 데이터 기반 스탯 적용
        MeleeActor->SetDamage(SkillData->Damage);
        MeleeActor->SetKnockbackForce(SkillData->KnockbackForce);
        MeleeActor->SetSweepRadius(SkillData->SweepRadius);

        if (AttackSound)
            UGameplayStatics::PlaySoundAtLocation(SkillOwner, AttackSound, SkillOwner->GetActorLocation());
        DrawDebugSphere(GetWorld(), SpawnLoc, SkillData->SweepRadius, 12, FColor::Green, 0, 1.0f, 0, 1.0f);
        // 공격 수행
        MeleeActor->PerformSweepAttack();
    }
    
}

void UDESkillBloodyNail::PerformConeSweep()
{
}
