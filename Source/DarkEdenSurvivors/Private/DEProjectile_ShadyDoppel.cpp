// Fill out your copyright notice in the Description page of Project Settings.


#include "DEProjectile_ShadyDoppel.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DESkillContext.h"
#include "GameFramework/Actor.h"

ADEProjectile_ShadyDoppel::ADEProjectile_ShadyDoppel()
{
    bIsPiercing = false;
    CurrentTime = 0.0f;
    TotalFlightTime = 1.0f;
}

void ADEProjectile_ShadyDoppel::InitializeFromContext(const FDESkillContext& Context)
{
    Super::InitializeFromContext(Context);

    if (MovementComponent)
    {
        MovementComponent->Velocity = FVector::ZeroVector;
        MovementComponent->Deactivate();
    }

    bIsPiercing = false;
    CurrentTime = 0.0f;
    StartPos = GetActorLocation();

    if (Context.Targets.Num() > 0 && Context.Targets[0])
    {
        TargetPos = Context.Targets[0]->GetActorLocation();
    }
    else
    {
        TargetPos = StartPos + (Context.TargetDirection.GetSafeNormal2D() * 1000.0f);
    }

    // [개선 1] 2D 기반으로 벡터 계산 최적화
    FVector DirToTarget = (TargetPos - StartPos).GetSafeNormal2D();

    // 외적 연산 없이 2D 직각 벡터(우측) 구하기: (X, Y) -> (Y, -X)
    FVector RightVector = FVector(DirToTarget.Y, -DirToTarget.X, 0.0f);

    float RandomOffset = FMath::RandRange(-CurveOffsetRange, CurveOffsetRange);

    // Dist2D를 사용하여 Z축 연산 제외
    ControlPos = StartPos + (DirToTarget * FVector::Dist2D(StartPos, TargetPos) * 0.5f) + (RightVector * RandomOffset);

    // [개선 2] 비행시간 나눗셈을 곱셈으로 바꾸기 위한 역수(Rate) 캐싱
    float Distance = FVector::Dist2D(StartPos, TargetPos);
    TotalFlightTime = Distance / FMath::Max(100.0f, CurrentSpeed);

    // 헤더에 멤버 변수로 선언 필요: float FlightRate;
    FlightRate = 1.0f / FMath::Max(0.01f, TotalFlightTime);
}

void ADEProjectile_ShadyDoppel::UpdateMovement(float DeltaTime)
{
    if (!bIsPiercing)
    {
        CurrentTime += DeltaTime;

        // [개선 2 적용] 나눗셈 대신 캐싱해둔 FlightRate 곱셈 사용
        float Alpha = CurrentTime * FlightRate;

        if (Alpha < 1.0f)
        {
            FVector L1 = FMath::Lerp(StartPos, ControlPos, Alpha);
            FVector L2 = FMath::Lerp(ControlPos, TargetPos, Alpha);
            FVector CurrentPos = FMath::Lerp(L1, L2, Alpha);

            // [개선 1 적용] 2D 정규화로 Z축 무시
            FVector MoveDir = (CurrentPos - GetActorLocation()).GetSafeNormal2D();

            // [개선 3 적용] 위치와 회전을 한 번의 함수 호출로 병합 (성능 대폭 향상)
            if (!MoveDir.IsNearlyZero())
            {
                SetActorLocationAndRotation(CurrentPos, MoveDir.Rotation());
            }
            else
            {
                SetActorLocation(CurrentPos);
            }
        }
        else
        {
            if (bCanPierceAfterCurve)
            {
                bIsPiercing = true;

                // [개선 1 적용] 탈출 방향도 2D 정규화
                ShootDirection = (TargetPos - ControlPos).GetSafeNormal2D();

                // 여기서도 병합하면 좋지만, Rotation만 바뀌므로 그대로 둡니다.
                SetActorRotation(ShootDirection.Rotation());

                if (MovementComponent)
                {
                    MovementComponent->Velocity = ShootDirection * CurrentSpeed;
                    MovementComponent->Activate();
                }
            }
            else
            {
                ReturnToPool();
            }
        }
    }
    else
    {
        Super::UpdateMovement(DeltaTime);
    }
}
