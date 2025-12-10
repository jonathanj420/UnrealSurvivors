// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkillAcidBolt.h"
#include "DEAcidBolt.h"
#include "Kismet/GameplayStatics.h"
#include "DEPoolSubsystem.h"

UDESkillAcidBolt::UDESkillAcidBolt()
{
    ProjectileClass = ADEAcidBolt::StaticClass();
    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Custom/AcidBolt1.AcidBolt1")
    );

    if (SoundObj.Succeeded())
    {
        FireSound = SoundObj.Object;
    }
}

void UDESkillAcidBolt::ActivateSkill(FDESkillData* SkillData)
{
    if (!SkillOwner || !SkillData || !ProjectileClass)
        return;

    int32 Count = SkillData->ProjectileCount;
    FVector Origin = SkillOwner->GetActorLocation();
    FRotator BaseRot = SkillOwner->GetActorRotation();

    // ¹ß»çÀ½
    if (FireSound)
        UGameplayStatics::PlaySoundAtLocation(SkillOwner, FireSound, Origin);

    float AngleStep = 360.f / Count;

    UDEPoolSubsystem* Pool = SkillOwner->GetWorld()->GetGameInstance()->GetSubsystem<UDEPoolSubsystem>();
    if (!Pool) return;

    for (int32 i = 0; i < Count; i++)
    {
        float Yaw = AngleStep * i;
        FRotator SpawnRot = BaseRot;
        SpawnRot.Yaw += Yaw;

        FVector Dir = SpawnRot.Vector();

        AActor* Pooled = Pool->GetPooledActor(
            ProjectileClass,
            Origin,
            SpawnRot,
            true
        );

        if (auto* Proj = Cast<ADEAcidBolt>(Pooled))
        {
            Proj->InitializeProjectile(
                SkillData->Damage,
                Proj->GetSpeed(),
                1,
                Dir
            );
        }
    }
}
