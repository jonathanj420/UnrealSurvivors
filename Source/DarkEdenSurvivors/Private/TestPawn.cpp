// Fill out your copyright notice in the Description page of Project Settings.


#include "TestPawn.h"
#include "ABAIController.h"
#include "Components/SphereComponent.h"

// Sets default values
ATestPawn::ATestPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // **1. 루트 컴포넌트 및 충돌체 설정 (몬스터의 히트박스)**
    CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    CapsuleComponent->InitCapsuleSize(42.0f, 96.0f); // 반지름, 높이
    CapsuleComponent->SetCollisionProfileName(TEXT("Monster"));
    RootComponent = CapsuleComponent;


    // **2. Attribute Component 생성**
    TestStatComponent = CreateDefaultSubobject<UTestStatComponent>(TEXT("AttributeComponent"));

    // **3. Floating Pawn Movement Component 생성**
    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovement"));

    // **4. 이동 속도 동기화 (AttributeComponent -> MovementComponent)**
    if (MovementComponent && TestStatComponent)
    {
        // AttributeComponent의 MoveSpeed 값을 MaxSpeed에 전달
        MovementComponent->MaxSpeed = TestStatComponent->GetMoveSpeed();
    }

    //  2. 스켈레탈 메시 컴포넌트 생성
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));

    //  3. 메시를 RootSphere(충돌체)에 부착
    MeshComponent->SetupAttachment(RootComponent);

    ConstructorHelpers::FObjectFinder<USkeletalMesh>SM_BKNIFE(TEXT("/Game/ZombieFemale/Asset/Meshes/ZombieFemale_NurseOutfit.ZombieFemale_NurseOutfit"));
    if (SM_BKNIFE.Succeeded())
    {
        MeshComponent->SetSkeletalMesh(SM_BKNIFE.Object);
    }
    //  4. 메시의 기본 회전 및 위치 조정 (선택 사항: 모델 정렬을 위해 필요할 수 있음)
    // MeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    // 
    // **5. AI 설정**
    //AIControllerClass = ATestAIController::StaticClass();
    //AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    //AIControllerClass = AABAIController::StaticClass();
    //AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

}

// Called when the game starts or when spawned
void ATestPawn::BeginPlay()
{
	Super::BeginPlay();
    TargetPlayer = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (nullptr!=TargetPlayer)
    {
        UE_LOG(LogTemp, Display, TEXT("Player imasu"));
    }
}

// Called every frame
void ATestPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    Dir = TargetPlayer->GetActorLocation() - GetActorLocation();
    Dir.Z = 0;     // Fix Z
    FVector Move = Dir.GetSafeNormal();


    FHitResult Hit;
    AddActorWorldOffset(Move * 300.0f * DeltaTime, true, &Hit);

    if (Hit.bBlockingHit)
    {
        //FString OtherActorName = Hit.GetActor()->GetName();
        //UE_LOG(LogTemp, Warning, TEXT("Hit : %s"), *OtherActorName);
        ////UE_LOG(LogTemp, Warning, TEXT("Hit Something"));

    }

}
