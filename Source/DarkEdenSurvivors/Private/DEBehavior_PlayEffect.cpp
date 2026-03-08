// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_PlayEffect.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DESkillContext.h"


void UDEBehavior_PlayEffect::Execute(FDESkillContext& Context)
{
    if (!Context.Instigator) return;

    UWorld* World = Context.Instigator->GetWorld();
    if (!World) return;

    // 1. 위치 & 어태치 타겟 수집
    TArray<FVector> SpawnLocations;
    TArray<AActor*> AttachTargets;

    switch (TargetType)
    {
    case EEffectTargetType::Instigator:
        SpawnLocations.Add(Context.Instigator->GetActorLocation());
        if (bAttachToActor) AttachTargets.Add(Context.Instigator);
        break;

    case EEffectTargetType::AllTargets:
        for (AActor* Target : Context.Targets)
        {
            if (!Target) continue;
            SpawnLocations.Add(Target->GetActorLocation());
            if (bAttachToActor) AttachTargets.Add(Target);
        }
        break;

    case EEffectTargetType::TargetCenter:
        if (Context.Targets.Num() > 0)
        {
            FVector Center = FVector::ZeroVector;
            int32 ValidCount = 0;
            for (AActor* Target : Context.Targets)
            {
                if (!Target) continue;
                Center += Target->GetActorLocation();
                ValidCount++;
            }
            if (ValidCount > 0)
                SpawnLocations.Add(Center / ValidCount);
        }
        break;

    case EEffectTargetType::CustomLocations:
        SpawnLocations = Context.CustomLocations;
        break;
    }

    // 2. 스폰
    for (int32 i = 0; i < SpawnLocations.Num(); i++)
    {
        FVector FinalPos = SpawnLocations[i] + Offset;
        AActor* TargetForRot = Context.Targets.IsValidIndex(i) ? Context.Targets[i] : nullptr;
        FRotator SpawnRot = GetSpawnRotation(Context, FinalPos, TargetForRot);

        // 나이아가라
        if (NiagaraEffect)
        {
            UNiagaraComponent* SpawnedComp = nullptr;

            if (bAttachToActor && AttachTargets.IsValidIndex(i))
            {
                SpawnedComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
                    NiagaraEffect,
                    AttachTargets[i]->GetRootComponent(),
                    NAME_None,
                    Offset,
                    SpawnRot,
                    EAttachLocation::KeepRelativeOffset,
                    bAutoDestroy,
                    true,
                    ENCPoolMethod::AutoRelease,
                    true
                );
            }
            else
            {
                SpawnedComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    World,
                    NiagaraEffect,
                    FinalPos,
                    SpawnRot,
                    FVector(1.f),
                    bAutoDestroy,
                    true,
                    ENCPoolMethod::AutoRelease,
                    true
                );
            }

            // 크기 주입
            if (SpawnedComp && !SizeVariableName.IsNone())
            {
                float FinalSize = FMath::Max(Context.Radius, 100.f) * SizeMultiplier;
                SpawnedComp->SetVariableFloat(SizeVariableName, FinalSize);
            }
        }

        // 사운드
        if (SoundEffect)
        {
            UGameplayStatics::PlaySoundAtLocation(World, SoundEffect, FinalPos);
        }
    }
}

FRotator UDEBehavior_PlayEffect::GetSpawnRotation(
    const FDESkillContext& Context, const FVector& SpawnLoc, AActor* Target) const
{
    switch (RotationType)
    {
    case EEffectRotation::InstigatorForward:
        return Context.Instigator->GetActorRotation();

    case EEffectRotation::TowardTarget:
        if (Target)
        {
            FVector Dir = Target->GetActorLocation() - SpawnLoc;
            return Dir.Rotation();
        }
        return FRotator::ZeroRotator;

    case EEffectRotation::RandomYaw:
        return FRotator(0.f, FMath::FRandRange(0.f, 360.f), 0.f);

    default:
        return FRotator::ZeroRotator;
    }
}