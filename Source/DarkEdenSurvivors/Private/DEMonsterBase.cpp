// Fill out your copyright notice in the Description page of Project Settings.


#include "DEMonsterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "DECharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "DEEXPCrystal.h"
#include "DEStatComponent.h"
#include "DEMonsterSpawnManager.h"

// Sets default values
ADEMonsterBase::ADEMonsterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->InitCapsuleSize(42.0f, 96.0f); // 반지름, 높이
	Capsule->SetCollisionProfileName(TEXT("Monster"));
	Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RootComponent = Capsule;


	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
	TestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TestMesh"));
	TestMesh->SetupAttachment(RootComponent);
	TestMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TestMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));
	ConstructorHelpers::FObjectFinder<UStaticMesh>SM_TESTMESH(TEXT("/Game/InfinityBladeWeapons/Weapons/Staff/StaticMesh/SM_Stf_StaffofAncients.SM_Stf_StaffofAncients"));
	if (SM_TESTMESH.Succeeded())
	{
		TestMesh->SetStaticMesh(SM_TESTMESH.Object);
	}

	EXPCrystal = ADEEXPCrystal::StaticClass();
	StatComp = CreateDefaultSubobject<UDEStatComponent>(TEXT("StatComponent"));
	StatComp->SetMaxHP(10.0f);

}

// Called when the game starts or when spawned
void ADEMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	StatComp->ResetStat();
	
	TargetPlayer = Cast<ADECharacterBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	for (TActorIterator<ADEMonsterSpawnManager> It(GetWorld()); It; ++It)
	{
		SpawnManager = *It;
		break; // 월드에 하나만 있다고 가정
	}

	if (!SpawnManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnManager not found!"));
	}
	StatComp->OnZeroHP.AddUObject(this, &ADEMonsterBase::Die);

}

// Called every frame
void ADEMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ADEMonsterBase::MoveToPlayer(float DeltaTime)
{
	if (!TargetPlayer) return;
	if (KnockbackVelocity != FVector::ZeroVector)
	{
		KnockbackVelocity = FMath::VInterpTo(
			KnockbackVelocity,
			FVector::ZeroVector,
			DeltaTime,
			KnockbackResistance);

		if (KnockbackVelocity.SizeSquared() < 1.0f)
		{
			KnockbackVelocity = FVector::ZeroVector;
		}
	}
	FVector Dir = TargetPlayer->GetActorLocation() - GetActorLocation();
	Dir.Z = 0.0f;             // Fix Z
	FVector MoveDelta = Dir.GetSafeNormal();
	FVector FinalMove = (MoveDelta * MoveSpeed + KnockbackVelocity) * DeltaTime;
	AddActorWorldOffset(FinalMove, false);
	//FHitResult Hit;
	//AddActorWorldOffset(FinalMove, false, &Hit);
	//
	//if (Hit.bBlockingHit)
	//{
	//	ADEMonsterBase* OtherMonster = Cast<ADEMonsterBase>(Hit.GetActor());
	//	if (OtherMonster)
	//	{
	//		// 넉백 힘 계산: 밀려나는 힘의 크기와 방향을 사용
	//		// 현재 몬스터(A)가 밀리던 속도(KnockbackVelocity)를 힘으로 사용합니다.

	//		// 부딪힌 몬스터(B)에게 넉백을 적용합니다.
	//		FVector ChainKnockbackDir = OtherMonster->GetActorLocation() - GetActorLocation();

	//		OtherMonster->ApplyKnockback(ChainKnockbackDir,KnockbackVelocity.Size()*0.5f); // 50%의 힘만 전달
	//	}
	//}

}


float ADEMonsterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	LOG_CALL();
	StatComp->TakeDamage(DamageAmount, DamageCauser);
	/*CurrentHP -= DamageAmount;
	UE_LOG(LogTemp, Warning, TEXT("Took : %f Damage , HP: %f"),DamageAmount, CurrentHP);

	if (CurrentHP <= 0.f)
	{
		CurrentHP = 0.0f;
		Die();
	}

	return DamageAmount;*/
	return DamageAmount;
}

void ADEMonsterBase::ApplyKnockback(const FVector& Direction, float Strength)
{
	FVector Dir = Direction;
	Dir.Z = 0.0f;
	Dir = Dir.GetSafeNormal();
	if (Dir.IsNearlyZero()) return;

	KnockbackVelocity += Dir*Strength;
}

void ADEMonsterBase::UpdateKnockback(float DeltaTime)
{
	// 부드럽게 0으로 감쇠 (VInterpTo)
	KnockbackVelocity = FMath::VInterpTo(KnockbackVelocity, FVector::ZeroVector, DeltaTime, KnockbackResistance);

	// 아주 작아지면 0으로
	if (KnockbackVelocity.SizeSquared() < 1.0f)
	{
		KnockbackVelocity = FVector::ZeroVector;
	}
}

void ADEMonsterBase::ResetForPool()
{
	UE_LOG(LogTemp, Warning, TEXT("Reset For Pool"));

	// Stat 초기화
	if (StatComp) StatComp->ResetStat();
	bIsDying = false;

	// 넉백 등 상태 초기화
	KnockbackVelocity = FVector::ZeroVector;



	// 비활성화: 숨기고 타이밍/충돌 끔
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}


void ADEMonsterBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}


float ADEMonsterBase::GetCollisionRadius() const
{
	if (Capsule) return Capsule->GetScaledCapsuleRadius();
	return 42.f; // default
}

float ADEMonsterBase::GetCapsuleHalfHeight() const
{
	if (Capsule) return Capsule->GetScaledCapsuleHalfHeight();
	return 96.f;
}

float ADEMonsterBase::GetMoveSpeed() const
{
	return MoveSpeed;
}

float ADEMonsterBase::GetDamage()
{
	return StatComp->GetDamage();
}

float ADEMonsterBase::GetCurrentHP()
{
	return StatComp->GetCurrentHP();
}

void ADEMonsterBase::DropExp()
{
	if (!EXPCrystal) return;

	int32 DropCount = FMath::RandRange(0, 1);
	DropCount = 1;
	for (int32 i = 0; i < DropCount; i++)
	{
		FVector Base = GetActorLocation();
		FVector Rand = FMath::VRand() * FMath::RandRange(50.f, 150.f);
		Rand.Z = FMath::RandRange(50.f, 200.f);

		FVector SpawnLoc = Base + Rand;

		GetWorld()->SpawnActor<ADEEXPCrystal>(
			EXPCrystal,
			SpawnLoc,
			FRotator::ZeroRotator
		);
	}

}

void ADEMonsterBase::ResetMonster()
{
	StatComp->ResetStat();
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	bIsAlive = true;
}

void ADEMonsterBase::Die()
{

	if (bIsDying) return;  // 이중 죽음 방지
	bIsDying = true;
	bIsAlive = false;

	DropExp();

	OnMonsterDeath.Broadcast(this);
}

bool ADEMonsterBase::IsAlive()
{
	
	return StatComp->GetCurrentHP() > 0.0f && bIsAlive;


}
void ADEMonsterBase::ApplyStun(float Duration)
{
	const float Now = GetWorld()->GetTimeSeconds();

	CCState = EMonsterCrowdControl::Stun;

	// ✔ 핵심: 기존 스턴보다 길면 갱신
	CCEndTime = FMath::Max(CCEndTime, Now + Duration);
}

bool ADEMonsterBase::IsStunned() const
{
	return CCState == EMonsterCrowdControl::Stun;
}

void ADEMonsterBase::UpdateCrowdControl(float CurrentTime)
{
	if (CCState == EMonsterCrowdControl::None)
		return;

	if (CurrentTime >= CCEndTime)
	{
		CCState = EMonsterCrowdControl::None;
		CCEndTime = 0.f;
	}
}
