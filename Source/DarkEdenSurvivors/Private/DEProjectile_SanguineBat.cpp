// Fill out your copyright notice in the Description page of Project Settings.


#include "DEProjectile_SanguineBat.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ADEProjectile_SanguineBat::ADEProjectile_SanguineBat()
{
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_BAT(TEXT("/Game/DarkEden/StaticMesh/Bat/emerald_bat/SkeletalMeshes/emerald_bat.emerald_bat"));
    if (SK_BAT.Succeeded())
    {
        SkeletalMesh->SetSkeletalMeshAsset(SK_BAT.Object);
        SkeletalMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
        //UE_LOG(LogTemp, Warning, TEXT("Bat Set"));
    }

    // 2. 부모 클래스에 있는 루트 컴포넌트(CollisionComponent)에 딱 붙여줍니다.
    if (CollisionComponent)
    {
        SkeletalMesh->SetupAttachment(CollisionComponent);
    }

    // 3. 부모님이 물려주신 StaticMesh는 안 쓸 거니까 확실하게 꺼버리기!
    if (Mesh)
    {
        Mesh->SetVisibility(false);
        Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Mesh->SetComponentTickEnabled(false); // 최적화 꿀팁
    }
}

void ADEProjectile_SanguineBat::OnTargetHit(AActor* Target)
{
    FVector ImpactNormal = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();

    ImpactNormal.Z = 0.0f; // 튕길 때 위로 솟구치지 않게 탑다운 유지
    ImpactNormal.Normalize();

    if (MovementComponent)
    {
        FVector NewVelocity = FMath::GetReflectionVector(MovementComponent->Velocity, ImpactNormal);
        MovementComponent->Velocity = NewVelocity;

        ShootDirection = NewVelocity.GetSafeNormal();
    }

    // 4. 무한 다단 히트 초기화
    // 다음 번 튕겨서 얘한테 또 왔을 때 다시 때릴 수 있게 세팅을 비워줍니다.
    HitActors.Reset();
}
