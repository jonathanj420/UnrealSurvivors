// Fill out your copyright notice in the Description page of Project Settings.


#include "DECharacterBase.h"
#include "DESkillManagerComponent.h"
#include "DEPlayerController.h"
#include "DEStatComponent.h"

// Sets default values
ADECharacterBase::ADECharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    StatComponent = CreateDefaultSubobject<UDEStatComponent>(TEXT("StatComponent"));
    StatComponent->SetMaxHP(200.0f);
    StatComponent->SetCurrentHP(200.0f);
    StatComponent->SetPlayer(true);
    SkillManager = CreateDefaultSubobject<UDESkillManagerComponent>(TEXT("SkillManager"));

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->TargetArmLength = 800.0f;
    SpringArm->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;
    ArmLengthSpeed = 3.0f;
    ArmRotationSpeed = 10.0f;
    IsFPS = false;
    bIsMovable = true;
    bMoveCamera = false;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;

    SetControlMode(EControlMode::FREETPS);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("TestCharacter"));

}

// Called when the game starts or when spawned
void ADECharacterBase::BeginPlay()
{
	Super::BeginPlay();
    if (IsPlayerControlled())
    {
        DEPlayerController = Cast<ADEPlayerController>(GetController());

    }

    if (StatComponent)
    {
        //StatComponent->OnLevelUp.AddDynamic(this, &ADECharacterBase::OnCharacterLevelUp);
        StatComponent->OnLevelUp.AddUObject(this, &ADECharacterBase::OnCharacterLevelUp);
    }
    if (SkillManager && BaseSkillID > 0)
    {
        SkillManager->LevelUpSkill(BaseSkillID);  // 추가할 함수 구현해줄 것
    }
    //StatComponent->OnLevelUp.AddDynamic(this, &ADEPlayerController::ShowLevelUpUI);
    SetControlMode(EControlMode::FIXEDTPS);
}

// Called every frame
void ADECharacterBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (bMoveCamera)
    {
        SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);
        SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));

        const float LengthTolerance = 1.0f;     // 길이 허용 오차 (1cm)
        const float RotationTolerance = 0.1f;   // 회전 허용 오차 (0.1도)

        // 암 길이가 목표에 도달했는지 확인
        bool bLengthReached = FMath::IsNearlyEqual(
            SpringArm->TargetArmLength,
            ArmLengthTo,
            LengthTolerance
        );

        // 회전이 목표에 도달했는지 확인
        bool bRotationReached = SpringArm->GetRelativeRotation().Equals(
            ArmRotationTo,
            RotationTolerance
        );

        if (bLengthReached && bRotationReached)
        {
            SpringArm->TargetArmLength = ArmLengthTo;
            SpringArm->SetRelativeRotation(ArmRotationTo);
            bMoveCamera = false;
            ABLOG(Warning, TEXT("Interp Done"));
        }
    }

}

void ADECharacterBase::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

// Called to bind functionality to input
void ADECharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("MoveForward", this, &ADECharacterBase::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ADECharacterBase::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &ADECharacterBase::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &ADECharacterBase::LookUp);

    //PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADECharacterBase::StartJump);
    //PlayerInputComponent->BindAction("Jump", IE_Released, this, &ADECharacterBase::StopJump);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ADECharacterBase::Jump);
    PlayerInputComponent->BindAction("ViewChange", IE_Pressed, this, &ADECharacterBase::ViewChange);

    PlayerInputComponent->BindAction("BloodDrain", IE_Pressed, this, &ADECharacterBase::BloodDrain);


}



void ADECharacterBase::MoveForward(float value)
{
    if (bIsMovable) {
        switch (CurrentControlMode)
        {
        case ADECharacterBase::EControlMode::FIXEDTPS:
            //DirectionToMove.X = value;
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), value);
            break;
        case ADECharacterBase::EControlMode::FREETPS:
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), value);
            break;
        case ADECharacterBase::EControlMode::FPS:
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), value);
            break;
        }
    }


}


void ADECharacterBase::MoveRight(float value)
{
    if (bIsMovable)
    {
        switch (CurrentControlMode)
        {
        case ADECharacterBase::EControlMode::FIXEDTPS:
            //DirectionToMove.Y = value;
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), value);
            break;
        case ADECharacterBase::EControlMode::FREETPS:
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), value);
            break;
        case ADECharacterBase::EControlMode::FPS:
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), value);
            break;
        }
    }



}

void ADECharacterBase::LookUp(float value)
{
    switch (CurrentControlMode)
    {
    case ADECharacterBase::EControlMode::FREETPS:
        AddControllerPitchInput(value);
        break;

    case ADECharacterBase::EControlMode::FPS:
        AddControllerPitchInput(value);
        break;
    }

}
void ADECharacterBase::Turn(float value)
{
    switch (CurrentControlMode)
    {
    case ADECharacterBase::EControlMode::FREETPS:
        AddControllerYawInput(value);
        break;

    case   ADECharacterBase::EControlMode::FPS:
        AddControllerYawInput(value);
        break;
    }

}

void ADECharacterBase::SetControlMode(EControlMode NewControlMode)
{
    CurrentControlMode = NewControlMode;

    switch (CurrentControlMode)
    {
    case EControlMode::FREETPS:
    {
        ArmRotationTo = FRotator(-60.0f, 0.0f, 0.0f);
        ArmLengthTo = 600.0f;
        SpringArm->bUsePawnControlRotation = true;
        SpringArm->bInheritPitch = true;
        SpringArm->bInheritRoll = true;
        SpringArm->bInheritYaw = true;
        SpringArm->bDoCollisionTest = true;
        bUseControllerRotationYaw = false;
        GetCharacterMovement()->bOrientRotationToMovement = true;
        GetCharacterMovement()->bUseControllerDesiredRotation = false;
        IsFPS = false;
        break;
    case EControlMode::FIXEDTPS:
        ArmLengthTo = 800.0f;
        ArmRotationTo = FRotator(-60.0f, 0.0f, 0.0f);
        SpringArm->bUsePawnControlRotation = false;
        SpringArm->bInheritPitch = false;
        SpringArm->bInheritRoll = false;
        SpringArm->bInheritYaw = false;
        SpringArm->bDoCollisionTest = false;
        bUseControllerRotationYaw = false;
        GetCharacterMovement()->bOrientRotationToMovement = true;
        GetCharacterMovement()->bUseControllerDesiredRotation = false;
        GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
        IsFPS = false;
        break;
    case EControlMode::FPS:
        ArmLengthTo = 0.0f;
        Camera->SetRelativeLocation(FVector(
            0.0f,
            0.0f,
            BaseEyeHeight
        ));
        //ArmRotationTo = FRotator::ZeroRotator;
        SpringArm->bUsePawnControlRotation = true; // 컨트롤러 회전 사용
        SpringArm->bInheritPitch = true;
        SpringArm->bInheritRoll = true;
        SpringArm->bInheritYaw = true;
        SpringArm->bDoCollisionTest = false; // 충돌 검사 불필요
        bUseControllerRotationYaw = true; // 컨트롤러의 좌우 움직임(Yaw)이 캐릭터 몸통을 회전시키도록 함
        GetCharacterMovement()->bOrientRotationToMovement = false; // 움직이는 방향으로 몸이 돌아가지 않도록 방지
        //GetCharacterMovement()->bUseControllerDesiredRotation = false; // (선택 사항)
        IsFPS = true;
        FRotator ControlRotation = GetControlRotation();

        // 2. Pitch(위/아래)와 Roll(기울임) 성분을 0으로 설정합니다.
        // 캐릭터의 몸체는 수평 방향(Yaw)만 회전해야 자연스럽습니다.
        ControlRotation.Pitch = 0.0f;
        ControlRotation.Roll = 0.0f;

        // 3. 이 회전 값을 캐릭터 액터에 적용합니다.
        // SetActorRotation은 월드 절대 회전을 설정합니다.
        SetActorRotation(ControlRotation);
        break;
    }
    }
    bMoveCamera = true;
    GetMesh()->SetOwnerNoSee(IsFPS);

}

void ADECharacterBase::ViewChange()
{
    switch (CurrentControlMode)
    {
    case ADECharacterBase::EControlMode::FIXEDTPS:
        GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
        SetControlMode(EControlMode::FREETPS);
        break;
    case ADECharacterBase::EControlMode::FREETPS:
        GetController()->SetControlRotation(GetActorRotation());
        SetControlMode(EControlMode::FPS);
        break;
    case ADECharacterBase::EControlMode::FPS:
        SetControlMode(EControlMode::FIXEDTPS);
        GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
        break;
    default:
        break;
    }
}

void ADECharacterBase::BloodDrain()
{
    //방폭 느낌 광흡
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("BloodDrain"));
    }


}

float ADECharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    StatComponent->TakeDamage(DamageAmount);
    return DamageAmount;
}

void ADECharacterBase::AddEXP(float v)
{
    StatComponent->AddEXP(v);
}

void ADECharacterBase::OnCharacterLevelUp()
{
    if (DEPlayerController)
    {
        DEPlayerController->ShowLevelUpUI(); // 인자 없는 버전이면 바로 호출
    }
}