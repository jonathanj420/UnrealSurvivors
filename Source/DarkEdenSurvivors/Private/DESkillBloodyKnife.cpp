// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillBloodyKnife.h"
#include "DEBloodyKnife.h"
#include "Kismet/GameplayStatics.h"
#include "DEPoolSubsystem.h"

//void UDESkillBloodyKnife::ActivateSkill(const FSkillSpec& Spec)
//{
//    UE_LOG(LogTemp, Warning, TEXT("Bloody Knife Projectile : %d by Self"), Spec.ProjectileCount);
//    if (!Spec.SkillActorClass || !SkillOwner) return;
//
//    CurrentSpec = Spec;
//    ShotsFired = 0;
//
//    // ※ 얼마나 연사할지는 원하는 대로 조절
//    MaxShots = 1;
//
//    // 일정 간격으로 FireSequence 반복
//    SkillOwner->GetWorldTimerManager().SetTimer(
//        FireTimerHandle,
//        this,
//        &UDESkillBloodyKnife::FireSequence,
//        0.5f,         // 연사 간격
//        true
//    );
//}

UDESkillBloodyKnife::UDESkillBloodyKnife()
{
    ProjectileClass = ADEBloodyKnife::StaticClass();
    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Vampire_BloodyKnife.Vampire_BloodyKnife")
    );

    if (SoundObj.Succeeded())
    {
        FireSound = SoundObj.Object;
    }
}

void UDESkillBloodyKnife::ActivateSkill(FDESkillData* SkillData)
{
//    UE_LOG(LogTemp, Warning, TEXT("%f seconds from last shot"), DeltaCheck);
    if (!SkillData || !SkillOwner) return;

    CurrentData = SkillData;
    //UE_LOG(LogTemp, Warning, TEXT("Bloody Knife Projectile : %d by Self"), CurrentData->ProjectileCount);
    ProjectileFired = 0;
    MaxProjectile = SkillData->ProjectileCount;

    /*SkillOwner->GetWorldTimerManager().SetTimer(
        FireTimerHandle,
        this,
        &UDESkillBloodyKnife::FireRandom,
        1.0f/SkillData->ProjectileCount,
        true
    );*/
    SkillOwner->GetWorldTimerManager().SetTimer(
        FireTimerHandle,
        this,
        &UDESkillBloodyKnife::FireRandom,
        0.1f,
        true
    );
}


void UDESkillBloodyKnife::FireRandom()
{
    if (ProjectileFired >= MaxProjectile)
    {
        SkillOwner->GetWorldTimerManager().ClearTimer(FireTimerHandle);
        return;
    }
    if (!SkillOwner || !CurrentData) return;
    UWorld* World = SkillOwner->GetWorld();
    if (!World) return;

    FVector Forward = SkillOwner->GetActorForwardVector();
    FVector Right = SkillOwner->GetActorRightVector();
    FVector Up = SkillOwner->GetActorUpVector();
    FVector OwnerLoc = SkillOwner->GetActorLocation();
    FRotator OwnerRot = SkillOwner->GetActorRotation();

    float RandRight = FMath::FRandRange(-40.f, 40.f);
    float RandUp = FMath::FRandRange(-40.f, 40.f);

    FVector SpawnLoc = OwnerLoc + Forward * 100.f + Right * RandRight + Up * RandUp;

    if (ProjectileClass)
    {

        UDEPoolSubsystem* Pool = SkillOwner->GetWorld()->GetGameInstance()->GetSubsystem<UDEPoolSubsystem>();
        if (!Pool) return;


        AActor* Pooled = Pool->GetPooledActor(
            ProjectileClass,
            SpawnLoc,
            OwnerRot,
            true
        );

        if (auto* Proj = Cast<ADEBloodyKnife>(Pooled))
        {
            // 3) ProjectileBase 스탯 초기화
            Proj->InitializeProjectile(
                CurrentData->Damage,
                Proj->GetSpeed(),
                CurrentData->Penetration,
                Forward
            );
        }
        if (FireSound)
        {
            UGameplayStatics::PlaySoundAtLocation(World, FireSound, SpawnLoc);
        }
        ProjectileFired++;
    }

}

//void UDESkillBloodyKnife::FireBurst()
//{
//    if (!SkillOwner) return;
//    UWorld* World = SkillOwner->GetWorld();
//    if (!World) return;
//
//    FVector Forward = SkillOwner->GetActorForwardVector();
//    FVector Right = SkillOwner->GetActorRightVector();
//    FVector Up = SkillOwner->GetActorUpVector();
//    FVector OwnerLoc = SkillOwner->GetActorLocation();
//    FRotator OwnerRot = SkillOwner->GetActorRotation();
//
//    // ProjectileCount만큼 동시에 발사
//    for (int32 i = 0; i < CurrentSpec.ProjectileCount; i++)
//    {
//        float RandRight = FMath::FRandRange(-40.f, 40.f);
//        float RandUp = FMath::FRandRange(-40.f, 40.f);
//
//        FVector SpawnLoc =
//            OwnerLoc +
//            Forward * 100.f +
//            Right * RandRight +
//            Up * RandUp;
//
//        World->SpawnActor<AActor>(
//            CurrentSpec.SkillActorClass,
//            SpawnLoc,
//            OwnerRot
//        );
//    }
//}
//void UDESkillBloodyKnife::FireBurst()
//{
//    if (!SkillOwner || !CurrentData) return;
//    UWorld* World = SkillOwner->GetWorld();
//    if (!World) return;
//
//    FVector Forward = SkillOwner->GetActorForwardVector();
//    FVector Right = SkillOwner->GetActorRightVector();
//    FVector Up = SkillOwner->GetActorUpVector();
//    FVector OwnerLoc = SkillOwner->GetActorLocation();
//    FRotator OwnerRot = SkillOwner->GetActorRotation();
//
//    SkillOwner->GetWorldTimerManager().SetTimer(
//        FireTimerHandle,
//        this,
//        &UDESkillBloodyKnife::FireSequence,
//        0.5f,
//        true
//    );
//
//    for (int32 i = 0; i < CurrentData->ProjectileCount; i++)
//    {
//        float RandRight = FMath::FRandRange(-40.f, 40.f);
//        float RandUp = FMath::FRandRange(-40.f, 40.f);
//
//        FVector SpawnLoc = OwnerLoc + Forward * 100.f + Right * RandRight + Up * RandUp;
//
//        World->SpawnActor<AActor>(CurrentData->SkillActorClass, SpawnLoc, OwnerRot);
//
//        if (CurrentData->SkillSound)
//        {
//            UGameplayStatics::PlaySoundAtLocation(World, CurrentData->SkillSound, SpawnLoc);
//        }
//    }
//}
//
//void UDESkillBloodyKnife::FireSequence()
//{
//    UE_LOG(LogTemp, Warning, TEXT("Fire Sequence"));
//    if (ShotsFired >= MaxShots)
//    {
//        SkillOwner->GetWorldTimerManager().ClearTimer(FireTimerHandle);
//        return;
//    }
//
//    FireBurst();
//    ShotsFired++;
//}
