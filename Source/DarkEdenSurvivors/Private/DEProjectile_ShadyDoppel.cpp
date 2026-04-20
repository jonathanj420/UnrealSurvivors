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

//void ADEProjectile_ShadyDoppel::InitializeFromContext(const FDESkillContext& Context)
//{
//    // 1. 부모 초기화 (여기서 데미지, 관통력, 기본 속도가 세팅됨)
//    Super::InitializeFromContext(Context);
//
//    // 2. 1페이즈(곡선) 동안은 부모의 직선 무브먼트 엔진을 잠시 끕니다.
//    if (MovementComponent)
//    {
//        MovementComponent->Velocity = FVector::ZeroVector;
//        MovementComponent->Deactivate();
//    }
//
//    // 3. 상태 및 시작 좌표 초기화
//    bIsPiercing = false;
//    CurrentTime = 0.0f;
//    StartPos = GetActorLocation();
//
//    // 4. 타겟 위치 설정
//    if (Context.Targets.Num() > 0 && Context.Targets[0])
//    {
//        TargetPos = Context.Targets[0]->GetActorLocation();
//    }
//    else
//    {
//        // 타겟이 없으면 바라보는 방향으로 적당히 멀리(1000) 찍어줌
//        TargetPos = StartPos + (Context.TargetDirection.GetSafeNormal() * 1000.0f);
//    }
//
//    // 5. 제어점(ControlPos) 비틀기 로직!
//    FVector DirToTarget = (TargetPos - StartPos).GetSafeNormal();
//    FVector RightVector = FVector::CrossProduct(FVector::UpVector, DirToTarget).GetSafeNormal();
//
//    // 왼쪽으로 꺾일지 오른쪽으로 꺾일지 랜덤하게 결정
//    float RandomOffset = FMath::RandRange(-CurveOffsetRange, CurveOffsetRange);
//
//    // 시작점과 타겟의 딱 중간 지점에서 좌/우로 RandomOffset만큼 밀어냄
//    ControlPos = StartPos + (DirToTarget * FVector::Distance(StartPos, TargetPos) * 0.5f) + (RightVector * RandomOffset);
//
//    // 6. [AAA 디테일] 거리에 맞춰서 비행시간 계산 (속도가 빠르면 빨리 도착함)
//    float Distance = FVector::Distance(StartPos, TargetPos);
//    TotalFlightTime = Distance / FMath::Max(100.0f, CurrentSpeed); // 0 나누기 방지
//}
//
//void ADEProjectile_ShadyDoppel::UpdateMovement(float DeltaTime)
//{
//    if (!bIsPiercing)
//    {
//        CurrentTime += DeltaTime;
//        float Alpha = CurrentTime / FMath::Max(0.01f, TotalFlightTime);
//
//        if (Alpha < 1.0f)
//        {
//            // (이전과 동일한 베지어 곡선 이동 로직) ...
//            FVector L1 = FMath::Lerp(StartPos, ControlPos, Alpha);
//            FVector L2 = FMath::Lerp(ControlPos, TargetPos, Alpha);
//            FVector CurrentPos = FMath::Lerp(L1, L2, Alpha);
//
//            FVector MoveDir = (CurrentPos - GetActorLocation()).GetSafeNormal();
//            if (!MoveDir.IsNearlyZero()) SetActorRotation(MoveDir.Rotation());
//            SetActorLocation(CurrentPos);
//        }
//        else
//        {
//            // ==========================================
//            // ★ 타겟 도달! (곡선 비행 종료)
//            // ==========================================
//            if (bCanPierceAfterCurve)
//            {
//                // [도플 2: 진화형] 우주 끝까지 직선 관통!
//                bIsPiercing = true;
//
//                ShootDirection = (TargetPos - ControlPos).GetSafeNormal();
//                SetActorRotation(ShootDirection.Rotation());
//
//                if (MovementComponent)
//                {
//                    MovementComponent->Velocity = ShootDirection * CurrentSpeed;
//                    MovementComponent->Activate();
//                }
//            }
//            else
//            {
//                // [도플 1: 일반형] 목표 지점에 도달했으니 깔끔하게 소멸!
//                // (펑! 하는 폭발 이펙트나 사운드를 여기서 재생해도 좋음)
//                ReturnToPool();
//            }
//        }
//    }
//    else
//    {
//        // 2페이즈: 진화형일 때만 실행되는 관통 비행
//        Super::UpdateMovement(DeltaTime);
//    }
//}
