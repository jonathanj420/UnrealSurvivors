// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_FireProjectileRadial.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "DESkillContext.h"
#include "DEPoolSubsystem.h"
#include "DESimpleProjectileBase.h"


void UDEBehavior_FireProjectileRadial::Execute(FDESkillContext& Context)
{


}

void UDEBehavior_FireProjectileRadial::FireProjectile(AActor* Instigator, const FVector& SpawnLocation, const FRotator& SpawnRotation)
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
