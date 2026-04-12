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
    // 공통 회전 로직 (중복 제거)
    //FRotator CurrentRot = GetActorRotation();
    //CurrentRot.Yaw += 720.f * DeltaTime; // 부메랑은 좀 더 빨리 돌아야 제맛!
    //SetActorRotation(CurrentRot);


    switch (Phase)
    {
    case EBoomerangPhase::Going:
    {
        // 1. [핵심] 가속도를 이용해 현재 속도를 점점 줄입니다.
        // RetractionAcceleration은 약 1000.0f 정도로 잡으시면 적당합니다.
        CurrentSpeed -= RetractionAcceleration * DeltaTime;

        // 2. 이동 처리
        FVector Delta = ShootDirection * CurrentSpeed * DeltaTime;
        AddActorWorldOffset(Delta);

        // 3. [상태 전환] 속도가 0 이하(정점)가 되면 Returning 페이즈로 전환!
        if (CurrentSpeed <= 0.0f)
        {
            Phase = EBoomerangPhase::Returning;
            HitActors.Reset();
            CurrentSpeed = 0.0f; // 0부터 다시 가속 시작하기 위해 초기화
        }
        break;
    }

    case EBoomerangPhase::Returning:
    {
        // 1. [핵심] 속도를 다시 가속시킵니다. (MaxSpeed까지)
        CurrentSpeed += RetractionAcceleration * DeltaTime;
        float FinalReturnSpeed = FMath::Min(CurrentSpeed, ReturnSpeed);

        FVector MoveDir;
        if (ReturnMode == EReturnMode::Linear || !CachedContext.Instigator)
        {
            MoveDir = -ShootDirection;
        }
        else // Homing
        {
            FVector ToOwner = CachedContext.Instigator->GetActorLocation() - GetActorLocation();
            MoveDir = ToOwner.GetSafeNormal();
        }

        // 2. 가속된 속도로 이동
        AddActorWorldOffset(MoveDir * FinalReturnSpeed * DeltaTime);

        // 3. 시전자 도달 체크 (기존 로직 유지)
        if (CachedContext.Instigator && bCanBeReturned)
        {
            float DistSq = FVector::DistSquared(GetActorLocation(), CachedContext.Instigator->GetActorLocation());
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