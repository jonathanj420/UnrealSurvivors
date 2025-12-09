// Fill out your copyright notice in the Description page of Project Settings.


#include "TestCharacter.h"
#include "Camera/CameraComponent.h"
#include "ABAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "ABWeapon.h"
#include "ABCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "ABCharacterWidget.h"
#include "ABAIController.h"
#include "Components/WidgetComponent.h"
#include "ABCharacterSetting.h"
#include "ABGameInstance.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABGameMode.h"
#include "ABHUDWidget.h"
#include "DEBloodyKnife.h"

// Sets default values
ATestCharacter::ATestCharacter()
{// 이 캐릭터가 프레임마다 Tick()을 호출하도록 설정합니다.  이 설정이 필요 없는 경우 비활성화하면 퍼포먼스가 향상됩니다.
    PrimaryActorTick.bCanEverTick = true;


    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
    SpringArm->SetupAttachment(GetRootComponent());
    SpringArm->TargetArmLength = 800.0f;
    SpringArm->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;

    CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
    HPBarWidget= CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
    HPBarWidget->SetupAttachment(GetMesh());


    ArmLengthSpeed = 3.0f;
    ArmRotationSpeed = 10.0f;
    IsFPS = false;



    GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_COUNTESS(TEXT("/Game/ParagonCountess/Characters/Heroes/Countess/Meshes/SM_Countess.SM_Countess"));
    if (SM_COUNTESS.Succeeded())
    {

        GetMesh()->SetSkeletalMesh(SM_COUNTESS.Object);
    }

    GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

    static ConstructorHelpers::FClassFinder<UAnimInstance> COUNTESS_ANIM(TEXT("/Game/ParagonCountess/Characters/Heroes/Countess/TestAnimBlueprint.TestAnimBlueprint_C"));
    if (COUNTESS_ANIM.Succeeded())
    {
        GetMesh()->SetAnimInstanceClass(COUNTESS_ANIM.Class);
    }

    SetControlMode(EControlMode::FREETPS);
    IsAttacking = false;
    AttackRange = 80.0f;
    AttackRadius = 50.0f;

    MaxCombo = 2;
    AttackEndComboState();
    bIsMovable = true;
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("TestCharacter"));

    FName WeaponLSocket(TEXT("weapon_lSocket"));
    FName WeaponRSocket(TEXT("weapon_rSocket"));
    //ABLOG(Warning, TEXT("Try Weapons"));
    /*if (GetMesh()->DoesSocketExist(WeaponRSocket) && GetMesh()->DoesSocketExist(WeaponLSocket))
    {
        ABLOG(Warning, TEXT("Sockets Exist"));
        WeaponL = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPONL"));
        WeaponR = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPONR"));
        static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("/Game/InfinityBladeWeapons/Weapons/Blunt/Blunt_Hambone/SK_Blunt_Hambone.SK_Blunt_Hambone"));
        if (SK_WEAPON.Succeeded())
        {
            ABLOG(Warning, TEXT("WEAPON SUCKSEX"));
            WeaponL->SetSkeletalMesh(SK_WEAPON.Object);
            WeaponR->SetSkeletalMesh(SK_WEAPON.Object);

        }
        WeaponL->SetupAttachment(GetMesh(), WeaponLSocket);
        WeaponR->SetupAttachment(GetMesh(), WeaponRSocket);
    }*/

    HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
    HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPBar.UI_HPBar_C"));
    if (UI_HUD.Succeeded())
    {
        HPBarWidget->SetWidgetClass(UI_HUD.Class);
        HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));

    }

    AIControllerClass = AABAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
    AssetIndex = 4;

    SetActorHiddenInGame(true);
    HPBarWidget->SetHiddenInGame(true);
    SetCanBeDamaged(false);


    DeadTimer = 5.0f;

    ProjectileClass = ADEBloodyKnife::StaticClass();

}
void ATestCharacter::SetCharacterState(ECharacterState NewState)
{
    ABCHECK(CurrentState != NewState);
    CurrentState = NewState;

    switch (CurrentState)
    {
    case ECharacterState::LOADING:
    {
        if (bIsPlayer)
        {
            DisableInput(ABPlayerController);

            ABPlayerController->GetHudWidget()->BindCharacterStat(CharacterStat);


            auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
            ABCHECK(nullptr != ABPlayerState);
            CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());

        }
        else
        {
            auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
            ABCHECK(nullptr != ABGameMode);
            int32 TargetLevel = FMath::CeilToInt(((float)ABGameMode->GetScore() * 0.8f));
            int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
            ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
        }


        SetActorHiddenInGame(true);
        HPBarWidget->SetHiddenInGame(true);
        SetCanBeDamaged(false);
        break;
    }
    case ECharacterState::READY:
    {
        SetActorHiddenInGame(false);
        HPBarWidget->SetHiddenInGame(false);
        SetCanBeDamaged(true);

        CharacterStat->OnHPIsZero.AddLambda([this]()->void {
            SetCharacterState(ECharacterState::DEAD);
            });

        auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
        ABCHECK(nullptr != HPBarWidget);
        CharacterWidget->BindCharacterStat(CharacterStat);

        if (bIsPlayer)
        {
            SetControlMode(EControlMode::FIXEDTPS);
            GetCharacterMovement()->MaxWalkSpeed = 600.0f;
            EnableInput(ABPlayerController);

        }
        else
        {
            SetControlMode(EControlMode::NPC);
            GetCharacterMovement()->MaxWalkSpeed = 300.0f;
            ABAIController->RunAI();

        }
        break;
    }
    case ECharacterState::DEAD:
    {
        SetActorEnableCollision(false);
        GetMesh()->SetHiddenInGame(false);
        HPBarWidget->SetHiddenInGame(true);
        ABAnim->SetDeadAnim();
        SetCanBeDamaged(false);

        if (bIsPlayer)
        {
            DisableInput(ABPlayerController);
        }
        else
        {
            ABAIController->StopAI();
        }
        GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void
            {
                if (bIsPlayer)
                {
                    ABPlayerController->ShowResultUI();
                }
                else
                {
                    Destroy();
                }
            }), DeadTimer, false);

        //CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        //CurrentWeapon->Destroy();
        //CurrentWeapon = nullptr;

        break;
    }
    }

}
ECharacterState ATestCharacter::GetCharacterState() const
{
    return CurrentState;
}
int32 ATestCharacter::GetExp() const
{
    return CharacterStat->GetDropExp();
}
float ATestCharacter::GetFinalAttackRange() const
{
    return (nullptr != CurrentWeapon) ? CurrentWeapon->GetAttackRange() : AttackRange;

}
float ATestCharacter::GetFinalAttackDamage() const
{
    float AttackDamage = (nullptr != CurrentWeapon) ? (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage()) : CharacterStat->GetAttack();
    float AttackModifier= (nullptr != CurrentWeapon) ? (CurrentWeapon->GetAttackModifier()) : 1.0f;
    return AttackDamage * AttackModifier;

}
// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();
    bIsPlayer = IsPlayerControlled();
    if (bIsPlayer)
    {
        ABPlayerController = Cast<AABPlayerController>(GetController());
        ABCHECK(nullptr != ABPlayerController);
    }
    else
    {
        ABAIController = Cast<AABAIController>(GetController());
        ABCHECK(nullptr != ABAIController);
    }

    auto DefaultSetting = GetDefault<UABCharacterSetting>();

    if (bIsPlayer)// 플레이어인지 ai인지 ? 플레이어 : 랜덤메시
    {
        auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
        ABCHECK(nullptr != ABPlayerState);
        AssetIndex = ABPlayerState->GetCharacterIndex();
    }
    else
    {
        AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
    }

    CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
    auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
    ABCHECK(nullptr != ABGameInstance);
    AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &ATestCharacter::OnAssetLoadCompleted)); //no need
    SetCharacterState(ECharacterState::LOADING);

    StartAutoAttack();
	
}

void ATestCharacter::SetControlMode(EControlMode NewControlMode)
{
    CurrentControlMode = NewControlMode;

    switch (CurrentControlMode)
    {
    case EControlMode::FREETPS:
    {
        //SpringArm->TargetArmLength = 600.0f;
        //SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
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
        // GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
        IsFPS = false;
        break;
    case EControlMode::FIXEDTPS:
        //SpringArm->TargetArmLength = 800.0f;
        //SpringArm->SetRelativeRotation(FRotator(-60.0f,0.0f,0.0f));
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
        ArmRotationTo = FRotator::ZeroRotator;
        SpringArm->bUsePawnControlRotation = true; // 컨트롤러 회전 사용
        SpringArm->bInheritPitch = true;
        SpringArm->bInheritRoll = true;
        SpringArm->bInheritYaw = true;
        SpringArm->bDoCollisionTest = false; // 충돌 검사 불필요
        bUseControllerRotationYaw = true; // 컨트롤러의 좌우 움직임(Yaw)이 캐릭터 몸통을 회전시키도록 함
        GetCharacterMovement()->bOrientRotationToMovement = false; // 움직이는 방향으로 몸이 돌아가지 않도록 방지
        GetCharacterMovement()->bUseControllerDesiredRotation = false; // (선택 사항)
        IsFPS = true;
        break;

    case EControlMode::NPC:
        bUseControllerRotationYaw = false;
        GetCharacterMovement()->bUseControllerDesiredRotation = false;
        GetCharacterMovement()->bOrientRotationToMovement = true;
        GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
        break;



    }
    }
    GetMesh()->SetOwnerNoSee(IsFPS);

}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (SpringArm)
    //{
    //    FRotator ArmRot = SpringArm->GetComponentRotation();
    //    FString RotText = FString::Printf(TEXT("SpringArm Rotation: %s"), *ArmRot.ToString());

    //    if (GEngine)
    //    {
    //        GEngine->AddOnScreenDebugMessage(
    //            -1,                // Key (-1이면 매 프레임마다 새 메시지)
    //            0.0f,              // Duration (0이면 한 프레임만 표시됨)
    //            FColor::Green,     // 색상
    //            RotText
    //        );
    //    }
    //}
    SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

   /* GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("Montage playing : %d"), ABAnim->IsAnyMontagePlaying()));
    GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("Current C0mbo: %d"),CurrentCombo));*/
    switch (CurrentControlMode)
    {
    case ATestCharacter::EControlMode::FIXEDTPS:
        //SpringArm->RelativeRotation = FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed);
        SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));

        break;
    case ATestCharacter::EControlMode::FREETPS:
        break;
    case ATestCharacter::EControlMode::FPS:
        break;
    default:
        break;
    }
   /* switch (CurrentControlMode)
    {
    case EControlMode::FIXEDTPS:
        if (DirectionToMove.SizeSquared() > 0.0f)
        {
            GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
            AddMovementInput(DirectionToMove);
        }
        break;

    }*/

    

}

void ATestCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
    ABCHECK(nullptr != ABAnim);

    ABAnim->OnMontageEnded.AddDynamic(this, &ATestCharacter::OnAttackMontageEnded);
    ABAnim->OnAttackHitCheck.AddUObject(this, &ATestCharacter::AttackCheck);


    ABAnim->OnNextAttackCheck.AddLambda([this]()->void {GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Lambda Activated")));
    CanNextCombo = false;

    if (IsComboInputOn)
    {
        AttackStartComboState();
        ABAnim->JumpToAttackMontageSection(CurrentCombo); //SPENT MY WHOLE MOFUCKIN LIFE TO NOTICE THIS MFUCKINSHITZ
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Pls Do Jump to %d"), CurrentCombo));
    }
        });

    CharacterStat->OnHPIsZero.AddLambda([this]()->void {

        ABLOG(Warning, TEXT("OnHPIsZero"));
        ABAnim->SetDeadAnim();
        SetActorEnableCollision(false);
        });

    

}

// Called to bind functionality to input
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATestCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATestCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &ATestCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &ATestCharacter::LookUp);

	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATestCharacter::StartJump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATestCharacter::StopJump);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATestCharacter::Jump);
    PlayerInputComponent->BindAction("ViewChange", IE_Pressed, this, &ATestCharacter::ViewChange);

    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ATestCharacter::BasicAttack);
    PlayerInputComponent->BindAction("BloodDrain", IE_Pressed, this, &ATestCharacter::BloodDrain);



}

float ATestCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);//EventInstigator=Controller(실질적 플레이어), DamageCauser=캐릭터 이름
    ABLOG(Warning, TEXT("Actor : %s took Damage : %f from : %s"), *GetName(), FinalDamage,*DamageCauser->GetName());
    
    CharacterStat->SetDamage(FinalDamage);
    if (CurrentState == ECharacterState::DEAD)
    {
        if (EventInstigator->IsPlayerController())
        {
            auto instigator = Cast<AABPlayerController>(EventInstigator);
            ABCHECK(nullptr != instigator, 0.0f);
            instigator->NPCKill(this);

        }
    }
    return FinalDamage;
}

void ATestCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (IsPlayerControlled())
    {

        SetControlMode(EControlMode::FREETPS);
        GetCharacterMovement()->MaxWalkSpeed = 600.0f;

    }
    else
    {
        SetControlMode(EControlMode::NPC);
        GetCharacterMovement()->MaxWalkSpeed = 300.0f;

    }
}


bool ATestCharacter::CanSetWeapon()
{
    //return (nullptr == CurrentWeapon);
    return true;


}

void ATestCharacter::SetWeapon(AABWeapon* NewWeapon)
{
    ABCHECK(nullptr != NewWeapon);

    if (nullptr != CurrentWeapon)
    {
        CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
    }
    ABCHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);
    FName WeaponSocketR(TEXT("weapon_rSocket"));
    FName WeaponSocketL(TEXT("weapon_lSocket"));
    if (nullptr != NewWeapon)
    {
        auto DupedWeapon = GetWorld()->SpawnActor<AABWeapon>(NewWeapon->GetClass());
        NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketR);
        DupedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketL);

        NewWeapon->SetOwner(this);
        DupedWeapon->SetOwner(this);
        CurrentWeapon = NewWeapon;


    }
}

void ATestCharacter::SetDualWeapon(AABWeapon* NewWeaponR, AABWeapon* NewWeaponL)
{
    if (nullptr != CurrentWeapon)
    {
        CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        CurrentWeapon->Destroy();
        CurrentWeapon = nullptr;
    }
    //나중에 듀얼웨폰

    ABLOG(Warning, TEXT("DUAL WIELDING"));
    ABCHECK(nullptr != NewWeaponR && nullptr!=NewWeaponL && nullptr == CurrentWeapon);
    FName WeaponSocketR(TEXT("weapon_rSocket"));
    FName WeaponSocketL(TEXT("weapon_lSocket"));
    if (nullptr != NewWeaponR && nullptr != NewWeaponL)
    {
        NewWeaponR->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketR);
        NewWeaponL->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketL);

        NewWeaponR->SetOwner(this);
        NewWeaponL->SetOwner(this);
        //CurrentWeapon = NewWeapon;


    }
}

void ATestCharacter::MoveForward(float value)
{
    if (bIsMovable) {
        switch (CurrentControlMode)
        {
        case ATestCharacter::EControlMode::FIXEDTPS:
            //DirectionToMove.X = value;
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), value);
            break;
        case ATestCharacter::EControlMode::FREETPS:
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), value);
            break;
        case ATestCharacter::EControlMode::FPS:
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), value);
            break;
        }
    }
    
        
}


void ATestCharacter::MoveRight(float value)
{
    if (bIsMovable)
    {
        switch (CurrentControlMode)
        {
        case ATestCharacter::EControlMode::FIXEDTPS:
            //DirectionToMove.Y = value;
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), value);
            break;
        case ATestCharacter::EControlMode::FREETPS:
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), value);
            break;
        case ATestCharacter::EControlMode::FPS:
            AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), value);
            break;
        }
    }
    


}

void ATestCharacter::LookUp(float value)
{
    switch (CurrentControlMode)
    {
    case ATestCharacter::EControlMode::FREETPS:
        AddControllerPitchInput(value);
        break;

    case ATestCharacter::EControlMode::FPS:
        AddControllerPitchInput(value);
        break;
    }

}
void ATestCharacter::Turn(float value)
{
    switch (CurrentControlMode)
    {
    case ATestCharacter::EControlMode::FREETPS:
        AddControllerYawInput(value);
        break;

    case   ATestCharacter::EControlMode::FPS:
        AddControllerYawInput(value);
        break;
    }

}


void ATestCharacter::StartJump()
{
	bPressedJump = true;

}

void ATestCharacter::StopJump()
{
	bPressedJump = false;
}

void ATestCharacter::ViewChange()
{
    switch (CurrentControlMode)
    {
    case ATestCharacter::EControlMode::FIXEDTPS:
        GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
        SetControlMode(EControlMode::FREETPS);
        break;
    case ATestCharacter::EControlMode::FREETPS:
        GetController()->SetControlRotation(GetActorRotation());
        SetControlMode(EControlMode::FPS);
        break;
    case ATestCharacter::EControlMode::FPS:
        SetControlMode(EControlMode::FIXEDTPS);
        GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
        break;
    default:
        break;
    }
}

void ATestCharacter::OnAssetLoadCompleted()
{
    //USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
    AssetStreamingHandle.Reset();
    /*ABCHECK(nullptr != AssetLoaded);
    if (nullptr != AssetLoaded)
    {
        GetMesh()->SetSkeletalMesh(AssetLoaded);
    }*/
    SetCharacterState(ECharacterState::READY);

}

void ATestCharacter::BasicAttack()
{
    //테스트용 평타
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Test Attack"));
    }
    bIsMovable = false;

    if (IsAttacking)
    {
        ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
        if (CanNextCombo)
        {
            IsComboInputOn = true;
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Is Combo"));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Is Not Combo"));
        ABCHECK(CurrentCombo == 0);
        AttackStartComboState();
        ABAnim->PlayAttackMontage();
        ABAnim->JumpToAttackMontageSection(CurrentCombo);
        IsAttacking = true;
    }


    //ABAnim->PlayAttackMontage();
    //IsAttacking = true;
    
}

void ATestCharacter::BulletAttack()
{
    ////실질적 뱀서 공격
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Vamp Surv Attack"));
    }
}

void ATestCharacter::BloodDrain()
{
    //방폭 느낌 광흡
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("BloodDrain"));
    }
    

}

void ATestCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("AttackMontageEnded")));
    ABCHECK(IsAttacking);
    ABCHECK(CurrentCombo > 0);
    IsAttacking = false;
    AttackEndComboState();
    bIsMovable = true;
    OnAttackEnd.Broadcast();


}

void ATestCharacter::AttackStartComboState()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("AttackStartComboState")));
    CanNextCombo = true;
    IsComboInputOn = false;
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("current combo : %d"),CurrentCombo));
    ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo-1));
    CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
   // GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("current combo : %d"), CurrentCombo));
}

void ATestCharacter::AttackEndComboState()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("AttackEndComboState")));
    }

    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("AttackEndComboState"))); Tq WTF
    IsComboInputOn = false;
    CanNextCombo = false;
    CurrentCombo = 0;

}

void ATestCharacter::AttackCheck()
{
    float FinalAttackRange = GetFinalAttackRange();

    FHitResult HitResult;
    FCollisionQueryParams Params(NAME_None, false, this);
    bool bResult = GetWorld()->SweepSingleByChannel(
        HitResult,
        GetActorLocation(), //중심점
        GetActorLocation() + GetActorForwardVector() * FinalAttackRange, //중심점+앞200거리= 200 떨어진 점
        FQuat::Identity, //회전?
        ECollisionChannel::ECC_GameTraceChannel2,
        FCollisionShape::MakeSphere(50.0f),
        Params);

#if ENABLE_DRAW_DEBUG
        FVector TraceVec = GetActorForwardVector() * FinalAttackRange; //앞방향으로 range만큼
        FVector Center = GetActorLocation() + TraceVec * 0.5f;
        float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
        FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
        FColor DrawColor = bResult ? FColor::Green : FColor::Red;
        float DebugLifeTime = 5.0f;

        DrawDebugCapsule(GetWorld(),
            Center,
            HalfHeight,
            AttackRadius,
            CapsuleRot,
            DrawColor,
            false,
            DebugLifeTime);
#endif


    if (bResult)
    {
        if (HitResult.GetActor())
        {
            ABLOG(Warning, TEXT("Hit Actor Name: %s"), *HitResult.GetActor()->GetName());

            FDamageEvent DamageEvent;
            //HitResult.GetActor()->TakeDamage(50.0f, DamageEvent, GetController(), this);
            HitResult.GetActor()->TakeDamage(GetFinalAttackDamage(), DamageEvent, GetController(), this);

        }
    }
}

void ATestCharacter::HandleAutoAttack()
{
    if (!ProjectileClass) return;

    FVector SpawnLoc = GetActorLocation() + GetActorForwardVector() * 100.f;
    FRotator SpawnRot = GetActorRotation();

    FActorSpawnParameters Params;
    GetWorld()->SpawnActor<ADEBloodyKnife>(ProjectileClass, SpawnLoc, SpawnRot, Params);
}

void ATestCharacter::StartAutoAttack()
{
    GetWorld()->GetTimerManager().SetTimer(
        AutoAttackTimerHandle,
        this,
        &ATestCharacter::HandleAutoAttack,
        AttackInterval,
        true
    );
}

void ATestCharacter::StopAutoAttack()
{
    GetWorld()->GetTimerManager().ClearTimer(AutoAttackTimerHandle);
}

//void ATestCharacter::BasicAttack()
//{
//    if (ProjectileClass)
//    {
//        FVector CameraLocation;
//        FRotator CameraRotation;
//        GetActorEyesViewPoint(CameraLocation, CameraRotation);
//
//
//        MuzzleOffset.Set(100.0f, 0.0f, 0.0f);
//
//        FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
//
//        FRotator MuzzleRotation = GetActorRotation();
//        MuzzleRotation.Pitch += 10.0f;
//
//        UWorld* World = GetWorld();
//
//        if (World)
//        {
//            FActorSpawnParameters SpawnParams;
//            SpawnParams.Owner = this;
//            SpawnParams.Instigator = GetInstigator();
//
//            // 총구에 발사체를 스폰합니다.
//            ATestProjectile* Projectile = World->SpawnActor<ATestProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
//            if (Projectile)
//            {
//                // 발사체의 초기 탄도를 설정합니다.
//                FVector LaunchDirection = MuzzleRotation.Vector();
//                Projectile->FireInDirection(LaunchDirection);
//            }
//        }
//    }
//}
