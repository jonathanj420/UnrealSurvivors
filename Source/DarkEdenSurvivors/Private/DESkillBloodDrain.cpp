// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillBloodDrain.h"

#include "DECharacterBase.h"
#include "DEMonsterBase.h"
#include "DEMonsterSpawnManager.h"

#include "DEGameMode.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

UDESkillBloodDrain::UDESkillBloodDrain()
{
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
        NS_BloodDrain(TEXT("/Game/DarkEden/Data/Niagara/NS_BloodDrainCustom.NS_BloodDrainCustom"));

    if (NS_BloodDrain.Succeeded())
    {
        BloodDrainEffect = NS_BloodDrain.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Vampire_Misc_BD.Vampire_Misc_BD")
    );

    if (SoundObj.Succeeded())
    {
        BloodDrainSound = SoundObj.Object;
    }
}

void UDESkillBloodDrain::ActivateSkill()
{
    ADECharacterBase* Player = Cast<ADECharacterBase>(SkillOwner);
    if (!Player)
        return;

    if (!Player->CanActivateBloodDrain())
        return;

    Player->ConsumeBloodDrainGauge();

    // 이동 / 자동공격 정지
    Player->SetCanMove(false);
    Player->SetAutoSkillsPaused(true);

    // 디버그
    DrawDebugSphere(
        GetWorld(),
        Player->GetActorLocation(),
        DrainRadius,
        16,
        FColor::Red,
        false,
        1.0f
    );

    StartBloodDrain(Player);

    if (BloodDrainSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            Player,
            BloodDrainSound,
            Player->GetActorLocation()
        );
    }

    // 종료 타이머
    GetWorld()->GetTimerManager().SetTimer(
        BloodDrainTimerHandle,
        this,
        &UDESkillBloodDrain::FinishBloodDrain,
        StunDuration,
        false
    );
}


void UDESkillBloodDrain::StartBloodDrain(ADECharacterBase* Player)
{
    CachedTargets.Empty();

    TArray<ADEMonsterBase*> Targets;
    CollectTargets(Targets);

    for (ADEMonsterBase* Monster : Targets)
    {
        if (!Monster || !Monster->IsAlive())
            continue;

        CachedTargets.Add(Monster);
        ApplyDrainEffect(Monster, Player);
    }
}

void UDESkillBloodDrain::FinishBloodDrain()
{
    ADECharacterBase* Player = Cast<ADECharacterBase>(SkillOwner);
    if (!Player)
        return;

    float TotalHeal = 0.f;

    for (TWeakObjectPtr<ADEMonsterBase> MonsterPtr : CachedTargets)
    {
        ADEMonsterBase* Monster = MonsterPtr.Get();
        if (!Monster || !Monster->IsAlive())
            continue;

        ApplyFinishDamage(Monster, Player);
        TotalHeal += DrainAmountPerMonster;
    }

    Player->Heal(TotalHeal);

    // 이동 / 자동공격 복구
    Player->SetCanMove(true);
    Player->SetAutoSkillsPaused(false);

    CachedTargets.Empty();
}

void UDESkillBloodDrain::CollectTargets(
    TArray<ADEMonsterBase*>& OutTargets) const
{
    const ADECharacterBase* Player = Cast<ADECharacterBase>(SkillOwner);
    if (!Player)
        return;

    const FVector PlayerLocation = Player->GetActorLocation();
    const float RadiusSq = DrainRadius * DrainRadius;

    ADEGameMode* GameMode = Player->GetWorld()->GetAuthGameMode<ADEGameMode>();
    if (!GameMode)
        return;

    ADEMonsterSpawnManager* SpawnManager =
        GameMode->GetMonsterSpawnManager();
    if (!SpawnManager)
        return;

    const TArray<ADEMonsterBase*>& Monsters =
        SpawnManager->GetActiveMonsters();

    for (ADEMonsterBase* Monster : Monsters)
    {
        if (!Monster || !Monster->IsAlive())
            continue;

        const float DistSq = FVector::DistSquared(
            PlayerLocation,
            Monster->GetActorLocation()
        );

        if (DistSq <= RadiusSq)
        {
            OutTargets.Add(Monster);
        }
    }
}

void UDESkillBloodDrain::ApplyDrainEffect(
    ADEMonsterBase* Monster,
    ADECharacterBase* Player
) const
{
    Monster->ApplyStun(StunDuration);

    if (BloodDrainEffect)
    {
        UNiagaraComponent* NiagaraComp =
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                Monster->GetWorld(),
                BloodDrainEffect,
                Monster->GetActorLocation()
            );

        if (NiagaraComp)
        {
            NiagaraComp->SetVectorParameter(
                TEXT("PlayerPosition"),
                Player->GetActorLocation()
            );
        }
    }
}

void UDESkillBloodDrain::ApplyFinishDamage(
    ADEMonsterBase* Monster,
    ADECharacterBase* Player
) const
{
    UGameplayStatics::ApplyDamage(
        Monster,
        DrainAmountPerMonster,
        Player->GetController(),
        Player,
        UDamageType::StaticClass()
    );
}