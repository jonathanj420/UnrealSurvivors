// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_BloodyKnife.h"
#include "DEProjectile_BloodyKnife.h"
#include "Kismet/GameplayStatics.h"
//#include "DEPoolSubsystem.h"
#include "DEBehavior_FireProjectile.h"



UDESkill_BloodyKnife::UDESkill_BloodyKnife()
{
    ProjectileClass = ADEProjectile_BloodyKnife::StaticClass();
    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Vampire_BloodyKnife.Vampire_BloodyKnife")
    );

    if (SoundObj.Succeeded())
    {
        FireSound = SoundObj.Object;
    }
}

void UDESkill_BloodyKnife::InitBehaviors()
{
    Super::InitBehaviors();
    // 나중에 여기에 각 스킬에 맞는 Behavior 추가하면 됨
    // 
    // 1. 행동(부품) 생성
    UDEBehavior_FireProjectile* FireAction = NewObject<UDEBehavior_FireProjectile>(this);

    // 2. 부품 설정 (이 스킬만의 특징 주입)
    FireAction->ProjectileClass = this->ProjectileClass; // "블러디 나이프를 던져라"
    FireAction->FireSound = this->FireSound;             // "이 소리를 내라"
    FireAction->BurstInterval = 0.1f;
    FireAction->RandomPositionRange = 40.0f;


    // 3. 장착
    Behaviors.Add(FireAction);
}

//void UDESkill_BloodyKnife::ActivateSkill(FDESkillData* SkillData)
//{
////    UE_LOG(LogTemp, Warning, TEXT("%f seconds from last shot"), DeltaCheck);
//    if (!SkillData || !SkillOwner) return;
//
//    CurrentData = SkillData;
//    //UE_LOG(LogTemp, Warning, TEXT("Bloody Knife Projectile : %d by Self"), CurrentData->ProjectileCount);
//    ProjectileFired = 0;
//    MaxProjectile = SkillData->ProjectileCount;
//
//    /*SkillOwner->GetWorldTimerManager().SetTimer(
//        FireTimerHandle,
//        this,
//        &UDESkill_BloodyKnife::FireRandom,
//        1.0f/SkillData->ProjectileCount,
//        true
//    );*/
//    SkillOwner->GetWorldTimerManager().SetTimer(
//        FireTimerHandle,
//        this,
//        &UDESkill_BloodyKnife::FireRandom,
//        0.1f,
//        true
//    );
//}


//void UDESkill_BloodyKnife::FireRandom()
//{
//    if (ProjectileFired >= MaxProjectile)
//    {
//        SkillOwner->GetWorldTimerManager().ClearTimer(FireTimerHandle);
//        return;
//    }
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
//    float RandRight = FMath::FRandRange(-40.f, 40.f);
//    float RandUp = FMath::FRandRange(-40.f, 40.f);
//
//    FVector SpawnLoc = OwnerLoc + Forward * 100.f + Right * RandRight + Up * RandUp;
//
//    if (ProjectileClass)
//    {
//
//        UDEPoolSubsystem* Pool = SkillOwner->GetWorld()->GetGameInstance()->GetSubsystem<UDEPoolSubsystem>();
//        if (!Pool) return;
//
//
//        AActor* Pooled = Pool->GetPooledActor(
//            ProjectileClass,
//            SpawnLoc,
//            OwnerRot,
//            true
//        );
//
//        if (auto* Proj = Cast<ADEProjectile_BloodyKnife>(Pooled))
//        {
//            // 3) ProjectileBase 스탯 초기화
//            Proj->InitializeProjectile(
//                CurrentData->Damage,
//                Proj->GetSpeed(),
//                CurrentData->Penetration,
//                Forward
//            );
//        }
//        if (FireSound)
//        {
//            UGameplayStatics::PlaySoundAtLocation(World, FireSound, SpawnLoc);
//        }
//        ProjectileFired++;
//    }
//
//}
