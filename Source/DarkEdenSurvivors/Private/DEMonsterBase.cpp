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
#include "DEHealthComponent.h"
#include "DEStatusEffectComponent.h"
#include "Data/DEMonsterData.h"
#include "DEDamageTypes.h"


// Sets default values
ADEMonsterBase::ADEMonsterBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add(TEXT("Enemy"));
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->InitCapsuleSize(42.0f, 96.0f); // 반지름, 높이
	Capsule->SetCollisionProfileName(TEXT("Monster"));
	RootComponent = Capsule;


	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	
	//TestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TestMesh"));
	//TestMesh->SetupAttachment(RootComponent);
	//TestMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//TestMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));
	//ConstructorHelpers::FObjectFinder<UStaticMesh>SM_TESTMESH(TEXT("/Game/InfinityBladeWeapons/Weapons/Staff/StaticMesh/SM_Stf_StaffofAncients.SM_Stf_StaffofAncients"));
	//if (SM_TESTMESH.Succeeded())
	//{
	//	TestMesh->SetStaticMesh(SM_TESTMESH.Object);
	//}

	EXPCrystal = ADEEXPCrystal::StaticClass();
	StatComponent = CreateDefaultSubobject<UDEStatComponent>(TEXT("StatComponent"));
	StatusEffectComponent = CreateDefaultSubobject<UDEStatusEffectComponent>(TEXT("StatusEffectComponent"));

	HealthComponent = CreateDefaultSubobject<UDEHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->SetMaxHP(100.0f, false);

}

// Called when the game starts or when spawned
void ADEMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	if (HealthComponent)
	{
		HealthComponent->ResetHealth();
		HealthComponent->OnDeath.AddUObject(this, &ADEMonsterBase::Die);

	}
	
	
	TargetPlayer = Cast<ADECharacterBase>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

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
	AddActorWorldOffset(FinalMove, true);
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

	// 1. 부모 클래스 로직 실행 (중요)
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 2. 체력 컴포넌트가 있다면 '우리의 시스템'으로 진입
	if (HealthComponent)
	{
		// [AAA 스타일] 요청서(Request) 작성
		FDEDamageRequest Req;

		// 2-1. 값 채우기
		Req.BaseDamage = ActualDamage;
		Req.DamageCauser = DamageCauser; // 때린 무기(투사체 or 칼)

		// ★ 중요: 언리얼 기본 함수는 Controller를 줍니다. 우린 Pawn(캐릭터)이 필요합니다.
		if (EventInstigator)
		{
			Req.Instigator = EventInstigator->GetPawn();
		}
		else
		{
			Req.Instigator = DamageCauser; // 컨트롤러가 없으면 그냥 때린 놈을 주범으로
		}

		// 2-2. 크리티컬 등 추가 정보 (기본 피격은 보통 크리 0%)
		Req.CritChance = 0.0f;
		Req.CritDamageMultiplier = 1.0f;

		// 3. 결재 올리기 (여기서 방어력 계산 등이 수행됨)
		FDEDamageResult Res = HealthComponent->ProcessDamage(Req);

		// 4. 실제로 들어간 데미지를 리턴 (언리얼 규칙 준수)
		return Res.FinalDamage;
	}

	return 0.0f;


	//old ahhh shiii
	////StatComp->TakeDamage(DamageAmount, DamageCauser);
	//HealthComponent->ApplyDamage(DamageAmount, DamageCauser);

	///*if (StatComp->GetCurrentHP() <= 0.f)
	//{
	//	Die();
	//}*/

	//return DamageAmount;
}

void ADEMonsterBase::ApplyKnockback(const FVector& Direction, float Strength)
{
	//UE_LOG(LogTemp, Warning, TEXT("Knockback : %f Applied"), Strength);
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

float ADEMonsterBase::GetDamage() const
{
	return AttackDamage;
}

float ADEMonsterBase::GetCurrentHP() const
{
	return HealthComponent->GetCurrentHP();
}

float ADEMonsterBase::GetMaxHP() const
{
	return HealthComponent->GetMaxHP();
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

void ADEMonsterBase::ResetMonster(const FDEMonsterData* Data)
{
	if (!Data) return;

	//mesh for later
	// 1. 메쉬 교체 (Soft Pointer 로딩)
	if (!Data->MonsterMesh.IsNull())
	{
		// 동기 로딩 (Synchronous Load). 
		// 뱀서류는 웨이브 시작 전 매니저가 미리 로딩해두는 게 정석이지만,
		// 구현 편의상 여기서 즉시 로딩해도 초기에는 문제 없음.
		USkeletalMesh* NewMesh = Data->MonsterMesh.LoadSynchronous();
		if (NewMesh)
		{
			Mesh->SetSkeletalMesh(NewMesh);
		}
	}


	//anim for later
	//// 2. 애니메이션 설정 (Single Node Mode)
	//if (!Data->WalkAnim.IsNull())
	//{
	//	UAnimSequence* NewAnim = Data->WalkAnim.LoadSynchronous();
	//	if (NewAnim)
	//	{
	//		// [핵심] AnimBP를 안 쓰고, 그냥 이 애니메이션만 주구장창 재생하라고 설정
	//		GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	//		GetMesh()->PlayAnimation(NewAnim, true); // true = Loop
	//	}
	//}
	//else
	//{
	//	// 애니메이션 없으면 T 포즈라도 잡게 초기화
	//	GetMesh()->SetAnimationMode(EAnimationMode::UseAnimationBlueprint);
	//}

	// 3. 캡슐 및 트랜스폼 보정
	Capsule->InitCapsuleSize(Data->CapsuleRadius, Data->CapsuleHalfHeight);

	// 메쉬 위치 보정 (발바닥 높이 맞추기)
	//GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, Data->MeshZOffset));

	// 전체 크기 보정
	//SetActorScale3D(FVector(Data->ScaleMultiplier));


	// 4. 스탯 적용 (기존과 동일)
	HealthComponent->SetMaxHP(Data->MaxHP,true);
	MoveSpeed = Data->MoveSpeed;
	AttackDamage = Data->AttackDamage;
	AttackInterval = Data->AttackInterval;
	KnockbackResistance = Data->KnockbackResistance;
	EXPDrop = Data->EXPDrop;
	DropChance = Data->DropChance;
	// 5. 상태 초기화
	HealthComponent->ResetHealth();
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	bIsAlive = true;
	bIsDying = false;
	if (HealthComponent) HealthComponent->ResetHealth(true);
}
void ADEMonsterBase::ResetForPool()
{
	//UE_LOG(LogTemp, Warning, TEXT("Reset For Pool"));

	// Stat 초기화
	//if (HealthComponent) HealthComponent->ResetHealth(true);
	//bIsDying = false;

	// 넉백 등 상태 초기화
	KnockbackVelocity = FVector::ZeroVector;



	// 비활성화: 숨기고 타이밍/충돌 끔
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void ADEMonsterBase::Die()
{

	if (bIsDying) return;  // 이중 죽음 방지
	bIsDying = true;
	bIsAlive = false;

	//DropExp();

	OnMonsterDeath.Broadcast(this);
}

bool ADEMonsterBase::IsAlive()
{
	
	return HealthComponent->IsAlive();


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

void ADEMonsterBase::NotifyActorBeginOverlap(AActor* OtherActor)
{

	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor->IsA(ADECharacterBase::StaticClass()))
	{
		bIsTouchingPlayer = true;
		OverlappingPlayer = Cast<ADECharacterBase>(OtherActor);
	}

	//Super::NotifyActorBeginOverlap(OtherActor);
	//// 1. 플레이어인지 확인
	//if (ADECharacterBase* Player = Cast<ADECharacterBase>(OtherActor))
	//{
	//	OverlappingPlayer = Player;
	//	bIsTouchingPlayer = true;
	//	// 2. 닿자마자 한 대 때림 (즉발)
	//	AttackPlayer();

	//	// 3. 계속 닿아있으면 0.5초마다 때리라고 타이머 켜기
	//	GetWorld()->GetTimerManager().SetTimer(
	//		AttackTimerHandle,
	//		this,
	//		&ADEMonsterBase::AttackPlayer,
	//		AttackInterval,
	//		true // 반복(Loop)
	//	);
	//} timer heavi
}

void ADEMonsterBase::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (OtherActor == OverlappingPlayer)
	{
		bIsTouchingPlayer = false;
		OverlappingPlayer = nullptr;
	}

	//Super::NotifyActorEndOverlap(OtherActor);

	//// 1. 플레이어가 나갔는지 확인
	//if (OtherActor == OverlappingPlayer)
	//{
	//	// 2. 타이머 끄기 (더 이상 안 때림)
	//	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	//	OverlappingPlayer = nullptr;
	//}
}

void ADEMonsterBase::ExecuteAttackLogic(double CurrentTime)
{
	// 1. [검사] 닿아있지 않거나, 대상이 없으면 리턴
	if (!bIsTouchingPlayer || !OverlappingPlayer) return;

	// 2. [검사] 대상이 죽었으면 리턴 (중요)
	if (OverlappingPlayer->IsDead()) return;

	// 3. [검사] 쿨타임 체크 (가장 중요)
	// (현재시간 - 마지막공격시간)이 0.5초보다 작으면 "아직 쿨타임 중"
	if (CurrentTime - LastAttackTime < AttackInterval)
		return;

	// --- [실행] 알맹이는 그대로 사용! ---
	UGameplayStatics::ApplyDamage(
		OverlappingPlayer,
		AttackDamage,
		nullptr,
		this,
		UDamageType::StaticClass()
	);

	// 4. [기록] "나 방금 때렸음" 도장 찍기
	LastAttackTime = CurrentTime;

	// (선택 사항) 공격 애니메이션 재생?
	// PlayAttackMontage();
}
