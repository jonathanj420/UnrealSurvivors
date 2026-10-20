// Fill out your copyright notice in the Description page of Project Settings.


#include "DECharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "DESkillManagerComponent.h"
#include "DEActiveSkillBase.h"
#include "DESkill_BloodDrain.h"
#include "DEStatComponent.h"
#include "DECombatComponent.h"
#include "DEHealthComponent.h"
#include "DEInventoryComponent.h"
#include "DEProgressionComponent.h"
#include "DEAccessoryComponent.h"
#include "DELevelUpManagerComponent.h"
#include "DEGameInstance.h"
#include "DEDamageTypes.h"
#include "DECharacterData.h"
#include "DEStatTypes.h"

// Sets default values
ADECharacterBase::ADECharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Tags.Add(FName("Player"));


    StatComponent = CreateDefaultSubobject<UDEStatComponent>(TEXT("StatComponent"));

    HealthComponent= CreateDefaultSubobject<UDEHealthComponent>(TEXT("DEHealthComponent"));

    CombatComponent = CreateDefaultSubobject<UDECombatComponent>(TEXT("CombatComponent"));

    ProgressionComponent = CreateDefaultSubobject<UDEProgressionComponent>(TEXT("ProgressionComponent"));

    SkillManager = CreateDefaultSubobject<UDESkillManagerComponent>(TEXT("SkillManager"));

    Inventory = CreateDefaultSubobject<UDEInventoryComponent>(TEXT("Inventory"));

    AccessoryComponent = CreateDefaultSubobject<UDEAccessoryComponent>(TEXT("AccessoryComponent"));

    LevelUpManagerComponent= CreateDefaultSubobject<UDELevelUpManagerComponent>(TEXT("LevelUpManagerComponent"));

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->TargetArmLength = 1600.0f;
    SpringArm->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
    SpringArm->bDoCollisionTest = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;
    ArmLengthSpeed = 3.0f;
    ArmRotationSpeed = 10.0f;
    IsFPS = false;
    bMoveCamera = false;
    bCanMove = true;
    GetCharacterMovement()->MaxWalkSpeed = 200.0f;

    SetControlMode(EControlMode::FREETPS);
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("DECharacter"));

    BloodDrainGauge=0.0f;
    BloodDrainGaugeMax=10.0f;
    BloodDrainGainPerKill=5.0f;
}

// Called when the game starts or when spawned
void ADECharacterBase::BeginPlay()
{
	Super::BeginPlay();

    if (StatComponent)
    {
        StatComponent->InitAsPlayer(this);
        if (SkillManager)
        {
            SkillManager->InitStatComp(StatComponent);
        }
        //StatComponent->OnLevelUp.AddDynamic(this, &ADECharacterBase::OnCharacterLevelUp);
        //StatComponent->OnLevelUp.AddUObject(this, &ADECharacterBase::OnCharacterLevelUp);
    }
    if (Inventory)
    {
        // (만약 인벤토리 컴포넌트 자체 초기화 함수가 있다면 여기서 호출)
        // InventoryComponent->InitInventory(...);

        if (SkillManager)
        {
            // 스킬 매니저에게 인벤토리 주머니를 쥐여줍니다.
            SkillManager->InitInventoryComp(Inventory);
        }
        if (AccessoryComponent)
        {
            AccessoryComponent->SetInventoryComp(Inventory);

        }
    }
    if (HealthComponent)
    {
        HealthComponent->OnDeath.AddUObject(this, &ADECharacterBase::Die);
    }
    if (ProgressionComponent)
    {
        // "야, 레벨업 하면 내 함수(OnLevelUp) 좀 실행해줘"
        ProgressionComponent->OnLevelUp.AddUObject(this, &ADECharacterBase::OnCharacterLevelUp);

        // 시작 레벨까지의 마일스톤 (RequiredLevel 1 = 시작부터)
        ApplyLevelMilestones(0, ProgressionComponent->GetCurrentLevel());
    }
    if (SkillManager && BaseSkillID > 0)
    {
        SkillManager->LevelUpSkill(BaseSkillID);  // 추가할 함수 구현해줄 것
    }
    SetControlMode(EControlMode::FIXEDTPS);
    ActiveSkill = NewObject<UDESkill_BloodDrain>(this);
    ActiveSkill->InitSkill(this);
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
        }
    }

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

    // 흡혈(BloodDrain)은 ActiveSkill 입력으로 발동한다
    PlayerInputComponent->BindAction("ActiveSkill", IE_Pressed, this, &ADECharacterBase::OnActiveSkillInput);

#if !UE_BUILD_SHIPPING
    PlayerInputComponent->BindAction("DebugCheat", IE_Pressed, this, &ADECharacterBase::MyDebugCheat);
    PlayerInputComponent->BindAction("AnotherDebugCheat", IE_Pressed, this, &ADECharacterBase::MyAnotherDebugCheat);
#endif

}



void ADECharacterBase::MoveForward(float value)
{
    if (!bCanMove) {
        return;
    }
    AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), value);

}


void ADECharacterBase::MoveRight(float value)
{
    if (!bCanMove) {
        return;
    }
    AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), value);



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
        ArmLengthTo = 1600.0f;
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
        ArmLengthTo = 1600.0f;
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

void ADECharacterBase::SetCanMove(bool bInCanMove)
{
    bCanMove = bInCanMove;

    if (!bCanMove)
    {
        GetCharacterMovement()->StopMovementImmediately();
    }
}
void ADECharacterBase::SetAutoSkillsPaused(bool bPaused)
{
    if (!SkillManager) return;

    if (bPaused)
    {
        SkillManager->PauseAutoSkills();
    }
    else
    {
        SkillManager->ResumeAutoSkills();
    }
}

float ADECharacterBase::GetBloodDrainGainPerKill()
{
    float BonusMultiplier = 1.0f;

    // TODO: 영구 업그레이드 적용
    // BonusMultiplier += BloodDrainUpgradePercent;

    return BloodDrainGainPerKill * BonusMultiplier;
}

void ADECharacterBase::AddBloodDrainGauge(float Amount)
{
    if (Amount <= 0.f) return;

    BloodDrainGauge = FMath::Clamp(
        BloodDrainGauge + Amount,
        0.f,
        BloodDrainGaugeMax
    );

}

void ADECharacterBase::ConsumeBloodDrainGauge()
{
    BloodDrainGauge = 0.f;
}

bool ADECharacterBase::CanActivateBloodDrain()
{
    return BloodDrainGauge >= BloodDrainGaugeMax;
}

float ADECharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (HealthComponent)
    {
        // 1. 단순 데미지도 구조체(Request)로 포장해서 처리 요청
        // 그래야 HealthComponent 안의 크리티컬/방어력 로직을 통과함
        FDEDamageRequest DmgRequest;
        DmgRequest.BaseDamage = ActualDamage;
        DmgRequest.DamageCauser = DamageCauser;
        DmgRequest.CritChance = 0.0f; // 일반 피격은 크리티컬 확률 0 (필요 시 수정)

        // 2. 통합된 처리 함수 호출
        //HealthComponent->ProcessDamage(DmgRequest);
    }

    return ActualDamage;
}

void ADECharacterBase::Heal(float Amount)
{
    if (HealthComponent)
    {
        HealthComponent->Heal(Amount);
    }

}

void ADECharacterBase::Die()
{
    // 1. 컨트롤러 입력 막기 (더 이상 움직이면 안 됨)
    //if (DEPlayerController)
    //{
    //    DisableInput(DEPlayerController);
    //}

    // 2. 충돌 끄기 (몬스터가 시체 그만 때리게)
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (OnPlayerDied.IsBound())
    {
        OnPlayerDied.Broadcast();
    }

    // 로그 확인
    UE_LOG(LogTemp, Warning, TEXT("Player Died!"));
}

bool ADECharacterBase::IsDead() const
{
    return HealthComponent && HealthComponent->IsDead();
}

void ADECharacterBase::AddExp(float v)
{
    if (ProgressionComponent)
    {
        ProgressionComponent->AddExp(v);
    }
}

void ADECharacterBase::SetMaxHP(float NewMaxHP)
{
    if (HealthComponent)
    {
        HealthComponent->SetMaxHP(NewMaxHP, false);
    }
}

void ADECharacterBase::ApplyLevelMilestones(int32 FromLevel, int32 ToLevel)
{
    if (!CharacterData || !StatComponent) return;

    for (const FPassiveMilestone& Milestone : CharacterData->LevelMilestones)
    {
        if (Milestone.RequiredLevel <= FromLevel || Milestone.RequiredLevel > ToLevel) continue;

        FDEStatModifier Mod;
        Mod.StatType = Milestone.StatType;
        Mod.Additive = Milestone.AdditiveBonus;
        Mod.Multiplier = Milestone.MultiplierBonus;
        StatComponent->ApplyModifier(Mod);
    }
}

void ADECharacterBase::OnCharacterLevelUp(int32 NewLevel)
{
    // ProgressionComponent는 여러 레벨을 한 번에 올려도 레벨마다 한 번씩 알린다
    ApplyLevelMilestones(NewLevel - 1, NewLevel);

    // 표시는 LevelUpManager 신호를 받은 컨트롤러가 한다
    if (LevelUpManagerComponent)
    {
        LevelUpManagerComponent->EnqueueLevelUp();
    }
}

void ADECharacterBase::OnActiveSkillInput()
{
    if (ActiveSkill)
    {
        ActiveSkill->ActivateSkill();
    }
}

float ADECharacterBase::GetCapsuleRadius() const
{
    return GetCapsuleComponent()->GetScaledCapsuleRadius();
}

void ADECharacterBase::MyDebugCheat()
{
#if !UE_BUILD_SHIPPING
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Cheat Activated"));
    }
    SkillManager->LevelUpSkill(BaseSkillID);
#endif
}

void ADECharacterBase::MyAnotherDebugCheat()
{
#if !UE_BUILD_SHIPPING
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Cheat Activated"));
    }
    TArray<int32> ConsumedSkillIDs;
    int32 ResultID = -1;

    // 진화 조건을 만족하는 무기가 있으면 바로 진화
    if (SkillManager->CheckEvolution(ConsumedSkillIDs, ResultID))
    {
        SkillManager->EvolveSkill(ConsumedSkillIDs, ResultID);
    }
#endif
}

void ADECharacterBase::ForceLevelUp()
{
#if !UE_BUILD_SHIPPING
    if (ProgressionComponent)
    {
        ProgressionComponent->LevelUp();
    }
#endif
}

void ADECharacterBase::ForceLevelUpSkill(int32 SkillId)
{
#if !UE_BUILD_SHIPPING
    if (SkillManager)
    {
        SkillManager->LevelUpSkill(SkillId);
    }
#endif
}
