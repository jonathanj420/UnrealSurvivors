// Fill out your copyright notice in the Description page of Project Settings.


#include "DEProjectile_Boomerang.h"
#include "GameFramework/ProjectileMovementComponent.h"

ADEProjectile_Boomerang::ADEProjectile_Boomerang()
{
    // 부메랑은 MovementComponent 안 씀
    if (MovementComponent)
    {
        if (MovementComponent)
        {
            MovementComponent->Velocity = FVector::ZeroVector;
            MovementComponent->bRotationFollowsVelocity = false;
            MovementComponent->Deactivate();
            MovementComponent->SetComponentTickEnabled(false);
        }
    }
}

void ADEProjectile_Boomerang::ResetState()
{
    Super::ResetState();

    MovementComponent->Deactivate();
    Phase = EBoomerangPhase::Going;
    TravelDistance = 0.f;
}

void ADEProjectile_Boomerang::UpdateMovement(float DeltaTime)
{
    switch (Phase)
    {
    case EBoomerangPhase::Going:
    {
        FVector Delta = ShootDirection * CurrentSpeed * DeltaTime;
        AddActorWorldOffset(Delta);
        TravelDistance += Delta.Size();

        // 회전 (도끼가 빙글빙글)
        FRotator CurrentRot = GetActorRotation();
        CurrentRot.Yaw += 360.f * DeltaTime;
        SetActorRotation(CurrentRot);

        if (TravelDistance >= MaxDistance)
        {
            Phase = EBoomerangPhase::Returning;
            HitActors.Empty(); // 복귀 시 다시 데미지 줄 수 있게

        }
        break;
    }

    case EBoomerangPhase::Returning:
    {
        FVector MoveDir;

        if (ReturnMode == EReturnMode::Linear)
        {
            MoveDir = -ShootDirection;

        }
        else // Homing
        {
            if (CachedContext.Instigator)
            {
                FVector ToOwner = CachedContext.Instigator->GetActorLocation()
                    - GetActorLocation();
                MoveDir = ToOwner.GetSafeNormal();
            }
            else
            {
                MoveDir = -ShootDirection;
            }
        }

        AddActorWorldOffset(MoveDir * ReturnSpeed * DeltaTime);

        // 회전
        FRotator CurrentRot = GetActorRotation();
        CurrentRot.Yaw += 360.f * DeltaTime;
        SetActorRotation(CurrentRot);

        // 시전자 도달 체크
        if (CachedContext.Instigator && bCanBeReturned)
        {
            float DistSq = FVector::DistSquared(
                GetActorLocation(),
                CachedContext.Instigator->GetActorLocation());

            if (DistSq <= FMath::Square(ReturnCompleteDistance))
                OnReturnComplete();
        }
        break;
    }
    }
}

void ADEProjectile_Boomerang::OnReturnComplete()
{
    UE_LOG(LogTemp, Warning, TEXT("Boomerang Returned"));
    ReturnToPool();
}