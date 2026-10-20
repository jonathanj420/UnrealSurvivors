// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAOE_OrbitBase.h"
#include "DESkillContext.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

ADEAOE_OrbitBase::ADEAOE_OrbitBase()
{
    PrimaryActorTick.bCanEverTick = true;

    // 계속 움직이므로 펄스 대신 닿는 순간 타격하고 타겟별 쿨다운을 둔다
    bHitOnContact = true;
}

void ADEAOE_OrbitBase::Tick(float DeltaTime)
{
    // 1. [매우 중요] 부모의 Tick을 실행해야 데미지를 줍니다!
    // 이게 없으면 빙빙 돌기만 하고 몬스터를 안 때립니다.
    Super::Tick(DeltaTime);

    // 2. 주인 확인 (GetOwner가 제일 안전)
    if (!GetOwner())
    {
        // 주인이 없으면 풀로 반납 (부모 클래스에 구현된 기능 사용)
        ReturnToPool();
        return;
    }

    // 3. 공전 계산 (시계 방향: -=, 반시계: +=)
    // OrbitSpeed(DT OptionValues)가 음수면 시계, 양수면 반시계로 돔
    CurrentAngle += RotationSpeed * DeltaTime;

    // 각도 정규화 (0~360 유지)
    if (CurrentAngle >= 360.0f) CurrentAngle -= 360.0f;
    if (CurrentAngle < 0.0f) CurrentAngle += 360.0f;

    // 4. 위치 이동 (극좌표 -> 직교좌표)
    FVector OwnerLoc = GetOwner()->GetActorLocation();
    float Rad = FMath::DegreesToRadians(CurrentAngle);

    FVector NewLoc;
    NewLoc.X = OwnerLoc.X + FMath::Cos(Rad) * CurrentOrbitRadius;
    NewLoc.Y = OwnerLoc.Y + FMath::Sin(Rad) * CurrentOrbitRadius;
    NewLoc.Z = OwnerLoc.Z; // 높이는 주인 따라가기

    //SetActorLocation(NewLoc);

    //// 5. 회전 (항상 바깥 보기, 혹은 진행방향 보기)
    //SetActorRotation(FRotator(0.0f, CurrentAngle, 0.0f)); // +- 90 

    // 3. 회전 방향 설정
    // bFaceDirection이라는 bool 변수를 헤더에 하나 파두고 입맛대로 골라 쓰시는 걸 추천합니다!
    FRotator NewRot;
    NewRot = FRotator(0.0f, CurrentAngle, 0.0f);

    SetActorLocationAndRotation(NewLoc, NewRot);

}


void ADEAOE_OrbitBase::InitOrbit(float InStartAngle)
{
    CurrentAngle = InStartAngle;
}

void ADEAOE_OrbitBase::InitializeFromContext(const FDESkillContext& Context)
{
    CurrentOrbitRadius = GetOrbitRadius(Context);
    RotationSpeed = GetOrbitSpeed(Context);

    Super::InitializeFromContext(Context);
}

float ADEAOE_OrbitBase::GetOrbitRadius(const FDESkillContext& Context)
{
    return Context.GetValue(TEXT("OrbitRadius"), 150.0f) * Context.FinalSnapshot.EffectSizeMultiplier;
}

float ADEAOE_OrbitBase::GetOrbitSpeed(const FDESkillContext& Context)
{
    return Context.GetValue(TEXT("OrbitSpeed"), 180.0f) * Context.FinalSnapshot.ProjectileSpeedMultiplier;
}
