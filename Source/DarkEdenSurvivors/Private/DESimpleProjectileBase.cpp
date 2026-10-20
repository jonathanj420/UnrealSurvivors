// Fill out your copyright notice in the Description page of Project Settings.


#include "DESimpleProjectileBase.h"
#include "DEMonsterQuerySubsystem.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DEPoolSubsystem.h"
#include "DEMonsterBase.h"
#include "DESkillContext.h"
#include "NiagaraComponent.h"
#include "DEDamageTypes.h"
#include "DEHealthComponent.h"
#include "DEGameplayLibrary.h"
#include "DEStatTypes.h"
#include "DEAutoSkillBase.h"
#include "DECombatEffect.h"


// Sets default values
ADESimpleProjectileBase::ADESimpleProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; // 투사체는 보통 틱이 필요합니다 (이동 등)

	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	//CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ADESimpleProjectileBase::OnOverlap);
	//CollisionComponent->SetCollisionProfileName(TEXT("NoCollision"));
	CollisionComponent->SetGenerateOverlapEvents(false);


	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->ProjectileGravityScale = 0.0f; // 기본 무중력
	MovementComponent->bSweepCollision = false; // [최적화 핵심] 이거 끄세요!

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovementComponent->SetUpdatedComponent(Mesh); // 루트(충돌체)를 돌림

	// 2. ★ [최적화 핵심] 기본적으로는 아예 꺼둡니다! ★
	RotatingMovementComponent->RotationRate = FRotator::ZeroRotator;
	RotatingMovementComponent->bAutoActivate = false;
	RotatingMovementComponent->SetComponentTickEnabled(false);

	// 4. [기본 변수 초기화]
	Penetration = 1; // 기본 1
	Damage = 0.0f;
	LifeTime = 3.0f;
 
}

// Called every frame
void ADESimpleProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateMovement(DeltaTime);

}

void ADESimpleProjectileBase::InitializeFromContext(const FDESkillContext& Context)
{

	// 1. 투사체 고유 데이터 세팅 (상자에서 방향 꺼내기!)
	ShootDirection = Context.TargetDirection.GetSafeNormal();
	Speed = Context.Speed;
	CurrentSpeed = (Speed == 0.0f) ? 1200.0f : Speed;
	Penetration = Context.Penetration;

	// 2. 속도 미리 주입
	if (MovementComponent)
	{
		MovementComponent->Velocity = ShootDirection * CurrentSpeed;
	}

	// 3. ★ 부모 초기화 호출 (이 안에서 가상 함수인 ResetState()가 호출되며 깔끔하게 1번만 켜짐)
	Super::InitializeFromContext(Context);

	// 히트 판정(PerformCollisionDetection)은 DT Radius로 한다. 콜리전 표시가 실제 판정 범위를 보여주도록 구체를 맞춘다.
	// 생성자의 InitSphereRadius 값은 판정에 쓰이지 않는다
	if (CollisionComponent)
	{
		CollisionComponent->SetSphereRadius(FMath::Max(Radius, 0.f));
	}
}

void ADESimpleProjectileBase::ResetState()
{
	Super::ResetState();
	// 4. 무브먼트 컴포넌트 활성화
	HitActors.Reset();

	if (MovementComponent)
	{
		MovementComponent->Activate();
		MovementComponent->SetUpdatedComponent(CollisionComponent); // 혹시 연결 끊겼을까봐
	}

	if (RotatingMovementComponent)
	{
		if (bEnableRotation)
		{
			RotatingMovementComponent->RotationRate = CustomRotationRate;
			RotatingMovementComponent->SetComponentTickEnabled(true);
			RotatingMovementComponent->Activate();
		}
		else
		{
			RotatingMovementComponent->RotationRate = FRotator::ZeroRotator;
			RotatingMovementComponent->SetComponentTickEnabled(false);
			RotatingMovementComponent->Deactivate();
		}
	}
	LastFrameLocation = GetActorLocation();
	//GetWorldTimerManager().SetTimer(LifeTimeTimerHandle, this, &ADESimpleProjectileBase::OnLifeTimeExpired, LifeTime, false);
}

void ADESimpleProjectileBase::ReturnToPool()
{
	if (MovementComponent)
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->Deactivate();
	}

	// 나머지는 부모가 알아서 끕니다.
	Super::ReturnToPool();
}

void ADESimpleProjectileBase::UpdateMovement(float DeltaTime)
{
	// [최적화] 가속도가 있을 때만 연산
	// KINDA_SMALL_NUMBER (0.0001) 체크도 비용이다. 그냥 0.0f 비교가 더 빠를 수도 있음.
	if (MovementComponent && Acceleration != 0.0f)
	{
		// 1. float 덧셈 (CPU가 제일 좋아하는 연산)
		CurrentSpeed += (Acceleration * DeltaTime);

		// 2. 음수 방지 & MaxSpeed 제한
		//if (CurrentSpeed < 0.0f) CurrentSpeed = 0.0f;
		// if (CurrentSpeed > MaxSpeed) ... (필요하면)

		// 3. [핵심] 제곱근 연산(sqrt) 없이 단순 곱셈으로 속도 적용
		MovementComponent->Velocity = ShootDirection * CurrentSpeed;
	}
}

void ADESimpleProjectileBase::PerformCollisionDetection(float DeltaTime)
{
	if (!CanHit()) return;

	if (Penetration == 0) return; // 관통력 다 썼으면 멈춤

	UWorld* World = GetWorld();
	if (!World) return;

	FVector StartLoc = LastFrameLocation;
	FVector EndLoc = GetActorLocation();

	const UDEMonsterQuerySubsystem* Query = World->GetSubsystem<UDEMonsterQuerySubsystem>();
	if (!Query) return;

	// 1. 이전 위치에서 현재 위치까지 쓸고 지나간 영역의 몬스터 (가까운 순)
	TArray<ADEMonsterBase*> HitMonsters;
	Query->QuerySegment(StartLoc, EndLoc, Radius, HitMonsters);

	for (ADEMonsterBase* Target : HitMonsters)
	{
		// 2. 이미 맞힌 대상은 건너뛴다 (관통 중 같은 적을 다시 때리지 않도록)
		if (HitCooldownMap.Contains(Target)) continue;

		// 3. 타격 처리
		if (TryDealDamage(Target))
		{
			// 타격 성공 시 명부에 이름 올리기 (투사체는 시간값을 안 쓰므로 0.0f 저장)
			HitCooldownMap.Add(Target, 0.0f);

			OnTargetHit(Target);

			// 관통력 감소 및 소멸 처리
			if (Penetration != -1 && --Penetration <= 0)
			{
				ReturnToPool();
				return;
			}
		}
	}

	// 4. 다음 프레임 스캔을 위해 현재 위치를 캐싱
	LastFrameLocation = EndLoc;
}

void ADESimpleProjectileBase::SetSpeed(float NewSpeed)
{
	// 1. 데이터 갱신
	Speed = NewSpeed;

	if (MovementComponent)
	{
		// 2. 무브먼트 설정 갱신 (다음 프레임부터 적용될 속도)
		MovementComponent->MaxSpeed = Speed;
		MovementComponent->InitialSpeed = Speed;

		// 3. ★ 현재 날아가고 있는 도중이라면, 즉시 속도 반영!
		if (MovementComponent->IsActive() && MovementComponent->Velocity.Size() > 0.f)
		{
			// 현재 방향(Direction)은 유지하되, 속력(Speed)만 갈아끼움
			MovementComponent->Velocity = MovementComponent->Velocity.GetSafeNormal() * Speed;
		}
	}

}

