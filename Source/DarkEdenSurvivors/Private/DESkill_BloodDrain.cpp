// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_BloodDrain.h"

#include "DECharacterBase.h"
#include "DEMonsterBase.h"
#include "DEMonsterSpawnManager.h"

#include "DEGameMode.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

UDESkill_BloodDrain::UDESkill_BloodDrain()
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

void UDESkill_BloodDrain::ActivateSkill()
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

    

    // 종료 타이머
    GetWorld()->GetTimerManager().SetTimer(
        BloodDrainTimerHandle,
        this,
        &UDESkill_BloodDrain::FinishBloodDrain,
        StunDuration,
        false
    );
}


void UDESkill_BloodDrain::StartBloodDrain(ADECharacterBase* Player)
{
    CachedTargets.Empty();

    TArray<ADEMonsterBase*> Targets;
    CollectTargets(Targets);

    if (Targets.Num() == 0)
    {
        return;

    }
    if (BloodDrainSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            Player,
            BloodDrainSound,
            Player->GetActorLocation()
        );
    }


    TArray<FVector> MonsterPositions;

    for (ADEMonsterBase* Monster : Targets)
    {
        if (!Monster || !Monster->IsAlive())
            continue;


        CachedTargets.Add(Monster);
        MonsterPositions.Add(Monster->GetActorLocation());
        Monster->ApplyStun(StunDuration);
        UE_LOG(LogTemp,Warning,TEXT("Monster Blood Drain posi : %s "),*Monster->GetActorLocation().ToString())

        
    }
    SpawnBloodDrainNiagara(Player, MonsterPositions);
}

void UDESkill_BloodDrain::FinishBloodDrain()
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

void UDESkill_BloodDrain::CollectTargets(
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

void UDESkill_BloodDrain::SpawnBloodDrainNiagara(
    ADECharacterBase* Player,
    const TArray<FVector>& MonsterPositions
) const
{
    if (!BloodDrainEffect)
        return;

    UNiagaraComponent* NiagaraComp =
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            Player->GetWorld(),
            BloodDrainEffect,
            Player->GetActorLocation()
        );

    if (!NiagaraComp)
        return;

    // 플레이어 위치
    NiagaraComp->SetVectorParameter(
        TEXT("PlayerPosition"),
        Player->GetActorLocation()
    );

    //// ⭐ 몬스터 위치 배열 전달
    //NiagaraComp->SetNiagaraVariableVec3Array(
    //    TEXT("User.MonsterPositions"),
    //    MonsterPositions
    //);
    //NiagaraComp->setvariablevec3array
    UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
        NiagaraComp,
        TEXT("MonsterPosi"), // 나이아가라 User Parameter 이름과 일치
        MonsterPositions);
    TArray<FVector> DebugArray=UNiagaraDataInterfaceArrayFunctionLibrary::GetNiagaraArrayVector(NiagaraComp, TEXT("MonsterPosi"));
    for (int i = 0; i < DebugArray.Num(); i++)
    {
        UE_LOG(LogTemp, Warning, TEXT("Monster Blood Drain Posi : %s"),*DebugArray[i].ToString());
    }

}

//void UDESkill_BloodDrain::ApplyDrainEffect(
//    ADEMonsterBase* Monster,
//    ADECharacterBase* Player
//) const
//{
//    Monster->ApplyStun(StunDuration);
//
//    if (BloodDrainEffect)
//    {
//        UNiagaraComponent* NiagaraComp =
//            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
//                Monster->GetWorld(),
//                BloodDrainEffect,
//                Monster->GetActorLocation()
//            );
//
//        if (NiagaraComp)
//        {
//            NiagaraComp->SetVectorParameter(
//                TEXT("PlayerPosition"),
//                Player->GetActorLocation()
//            );
//        }
//    }
//
//}

void UDESkill_BloodDrain::ApplyFinishDamage(
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