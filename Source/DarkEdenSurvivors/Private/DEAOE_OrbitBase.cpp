// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAOE_OrbitBase.h"
#include "DESkillContext.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

ADEAOE_OrbitBase::ADEAOE_OrbitBase()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    // 2. 부모의 콜리전 컴포넌트 하위에 부착 (Collision이 Root라고 가정)
    // 만약 Collision이 Root가 아니라면 GetRootComponent()에 부착하세요.
    if (Collision)
    {
        MeshComp->SetupAttachment(Collision);
    }
    else
    {
        // 혹시 모를 안전장치: Collision이 없으면 루트로 설정
        SetRootComponent(MeshComp);
    }

    // 3. [중요] 메시 자체의 충돌은 끕니다.
    // 실제 데미지 판정은 SphereComponent(Collision)가 담당하므로 중복 계산을 막습니다.
    MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
    MeshComp->SetGenerateOverlapEvents(false);
    MeshComp->SetCanEverAffectNavigation(false); // 내비게이션 영향 X
}

void ADEAOE_OrbitBase::Tick(float DeltaTime)
{
    // 1. [매우 중요] 부모의 Tick을 실행해야 데미지를 줍니다!
    // 이게 없으면 빙빙 돌기만 하고 몬스터를 안 때립니다.
    Super::Tick(DeltaTime);

    // 2. 주인 확인 (GetOwner가 제일 안전)
    OwnerActor = GetOwner();
    if (!OwnerActor.IsValid())
    {
        // 주인이 없으면 풀로 반납 (부모 클래스에 구현된 기능 사용)
        ReturnToPool();
        return;
    }

    // 3. 공전 계산 (시계 방향: -=, 반시계: +=)
    // 데이터 테이블 Speed가 음수면 시계, 양수면 반시계로 돔
    CurrentAngle += RotationSpeed * DeltaTime;

    // 각도 정규화 (0~360 유지)
    if (CurrentAngle >= 360.0f) CurrentAngle -= 360.0f;
    if (CurrentAngle < 0.0f) CurrentAngle += 360.0f;

    // 4. 위치 이동 (극좌표 -> 직교좌표)
    FVector OwnerLoc = OwnerActor->GetActorLocation();
    float Rad = FMath::DegreesToRadians(CurrentAngle);

    FVector NewLoc;
    NewLoc.X = OwnerLoc.X + FMath::Cos(Rad) * CurrentOrbitRadius;
    NewLoc.Y = OwnerLoc.Y + FMath::Sin(Rad) * CurrentOrbitRadius;
    NewLoc.Z = OwnerLoc.Z; // 높이는 주인 따라가기

    SetActorLocation(NewLoc);

    // 5. 회전 (항상 바깥 보기, 혹은 진행방향 보기)
    SetActorRotation(FRotator(0.0f, CurrentAngle, 0.0f)); // +- 90 

    //Super::Tick(DeltaTime);

    //OwnerActor = GetOwner();
    //if (!OwnerActor)
    //{
    //    Destroy();
    //    return;
    //}

    //// 공전 로직
    //CurrentAngle -= RotationSpeed * DeltaTime;
    //if (CurrentAngle < 0.0f) CurrentAngle += 360.0f;

    //FVector OwnerLoc = OwnerActor->GetActorLocation();
    //float Rad = FMath::DegreesToRadians(CurrentAngle);

    //float NewX = OwnerLoc.X + FMath::Cos(Rad) * CurrentOrbitRadius;
    //float NewY = OwnerLoc.Y + FMath::Sin(Rad) * CurrentOrbitRadius;

    //// AOE니까 바닥이나 허리춤에 위치
    //SetActorLocation(FVector(NewX, NewY, OwnerLoc.Z));

    //// (선택) 항상 바깥 보기
    //SetActorRotation(FRotator(0.0f, CurrentAngle, 0.0f));
}

void ADEAOE_OrbitBase::ApplyContext(const FDESkillContext& Context)
{
    // 1. [중요] 부모 함수 먼저 호출! 
    // 여기서 Damage, Duration, OwnerActor 설정이 다 끝납니다.
    Super::ApplyContext(Context);

    // 2. 공전(Orbit) 전용 스탯만 추가로 읽어옴
    float OrbitRadius = Context.GetValue(TEXT("OrbitRadius"), 150.0f);
    float AreaMult = Context.GetValue(TEXT("AreaMultiplier"), 1.0f);

    // 범위가 커지면 더 멀리서 돔 (최대 거리 제한)
    CurrentOrbitRadius = FMath::Min(OrbitRadius * AreaMult, MaxRadius);

    // 속도 배율
    float SpeedMult = Context.GetValue(TEXT("SpeedMultiplier"), 1.0f);
    RotationSpeed = Context.GetValue(TEXT("OrbitSpeed"), 180.0f) * SpeedMult;

    // 3. 크기(Scale) 적용
    // 부모 ApplyContext에도 Scale 로직이 있지만, AreaMultiplier로 확실하게 한 번 더 덮어씀
    SetActorScale3D(FVector(AreaMult));


    //Super::ApplyContext(Context); // 부모(AOEBase)의 데미지, 쿨타임 로직 먼저 수행

    //// 공전 관련 스탯만 추가로 읽어옴
    //float BaseRadius = Context.GetValue(TEXT("BaseRadius"), 150.0f);
    //float AreaMult = Context.GetValue(TEXT("AreaMultiplier"), 1.0f);

    //CurrentOrbitRadius = FMath::Min(BaseRadius * AreaMult, MaxRadius);

    //float SpeedMult = Context.GetValue(TEXT("SpeedMultiplier"), 1.0f);
    //RotationSpeed = Context.GetValue(TEXT("OrbitSpeed"), 180.0f) * SpeedMult;

    //SetActorScale3D(FVector(AreaMult));
}

void ADEAOE_OrbitBase::InitOrbit(float InStartAngle)
{
    CurrentAngle = InStartAngle;
}
