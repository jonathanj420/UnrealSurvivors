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

    // ---------------------------------------------------------
    // 1. 나이아가라 이펙트 처리 (시각 효과 - 타겟 수만큼 복사)
    // ---------------------------------------------------------
    if (NiagaraEffect)
    {
        TArray<FVector> NiagaraLocs = ResolveLocations(NiagaraTargetType, Context);

        // 어태치 타겟 수집 (나이아가라 타겟 타입이 Instigator나 AllTargets일 때만 적용)
        TArray<AActor*> AttachTargets;
        if (bAttachToActor)
        {
            if (NiagaraTargetType == EEffectTargetType::Instigator)
            {
                AttachTargets.Add(Context.Instigator);
            }
            else if (NiagaraTargetType == EEffectTargetType::AllTargets)
            {
                for (AActor* Target : Context.Targets)
                {
                    if (Target) AttachTargets.Add(Target);
                }
            }
        }

        // 수집된 위치마다 나이아가라
        for (int32 i = 0; i < NiagaraLocs.Num(); i++)
        {
            FVector FinalPos = NiagaraLocs[i] + Offset;
            AActor* TargetForRot = Context.Targets.IsValidIndex(i) ? Context.Targets[i] : nullptr;
            FRotator SpawnRot = GetSpawnRotation(Context, FinalPos, TargetForRot);

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

            if (SpawnedComp && TargetForRot) // 개발자님이 이미 만들어둔 완벽한 변수 재활용!
            {
                // 나이아가라의 User.TargetPosition 에 몬스터의 절대 좌표를 꽂습니다!
                SpawnedComp->SetVariableVec3(FName("TargetPosition"), TargetForRot->GetActorLocation());
            }
            if (SpawnedComp)
            {
                // 1. 발사 시작점
                FVector StartLoc = Context.Instigator->GetActorLocation();

                // 2. 방향 계산 (타겟이 있으면 타겟 방향, 없으면 전방)
                FVector Direction = Context.Instigator->GetActorForwardVector();
                if (Context.Targets.Num() > 0 && Context.Targets[0])
                {
                    Direction = (Context.Targets[0]->GetActorLocation() - StartLoc).GetSafeNormal2D();
                }

                // 3. 거리 계산 (Context에 설정된 Range 사용)
                float FinalRange = Context.GetValue(TEXT("Range"), 1000.0f); // 스킬 스펙의 Range 가져오기
                UE_LOG(LogTemp, Warning, TEXT("Niagara Beam Range : %f"), FinalRange);
                // 4. 최종 끝점 좌표 계산 (시작점 + (방향 * 거리))
                FVector BeamEndLoc = StartLoc + (Direction * FinalRange);

                // 5. 나이아가라에 "BeamEnd"라는 이름으로 좌표 쏴주기
                SpawnedComp->SetVariableVec3(FName("BeamEnd"), BeamEndLoc);
            }
        }
        //UE_LOG(LogTemp, Warning, TEXT("NiagaraLocs count: %d"), NiagaraLocs.Num());
    }

    // ---------------------------------------------------------
    // 2. 사운드 처리 (청각 효과 - 고막 보호를 위해 무조건 1번만!)
    // ---------------------------------------------------------
    if (SoundEffect)
    {
        if (SoundTargetType == EEffectTargetType::Instigator)
        {
            // 시전자 기준: 2D 사운드로 화면 전체에 쩌렁쩌렁하게!
            UGameplayStatics::PlaySound2D(World, SoundEffect);
        }
        else
        {
            // 그 외 기준: 위치를 수집해서 3D 사운드로 1번만 재생!
            TArray<FVector> SoundLocs = ResolveLocations(SoundTargetType, Context);
            if (SoundLocs.Num() > 0)
            {
                // 적이 300마리여도 첫 번째 위치에서 딱 1번만 소리가 납니다.
                UGameplayStatics::PlaySoundAtLocation(World, SoundEffect, SoundLocs[0]);
            }
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

TArray<FVector> UDEBehavior_PlayEffect::ResolveLocations(EEffectTargetType InTargetType, const FDESkillContext& Context) const
{
    TArray<FVector> Locs;
    switch (InTargetType)
    {
    case EEffectTargetType::Instigator:
        if (Context.Instigator) Locs.Add(Context.Instigator->GetActorLocation());
        break;

    case EEffectTargetType::AllTargets:
        for (AActor* Target : Context.Targets)
        {
            if (Target) Locs.Add(Target->GetActorLocation());
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
            if (ValidCount > 0) Locs.Add(Center / ValidCount);
        }
        break;

    case EEffectTargetType::CustomLocations:
        Locs = Context.CustomLocations;
        break;
    }
    return Locs;
}
