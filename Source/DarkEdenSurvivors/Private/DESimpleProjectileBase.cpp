// Fill out your copyright notice in the Description page of Project Settings.


#include "DESimpleProjectileBase.h"
#include "Components/SphereComponent.h"
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

// Sets default values
ADESimpleProjectileBase::ADESimpleProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; // 투사체는 보통 틱이 필요합니다 (이동 등)

	// 1. [충돌체 생성] (공통)
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	RootComponent = CollisionComponent;
	CollisionComponent->InitSphereRadius(30.0f); // 기본값
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));

	// ★ 중요: OnOverlap 연결은 부모에서 한 번만 하면 됩니다!
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ADESimpleProjectileBase::OnOverlap);

	// 2. [메시 생성] (공통)
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 메시는 장식용
    Mesh->SetSimulatePhysics(false);
    Mesh->SetEnableGravity(false);
	// 3. [무브먼트 생성] (공통)

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->ProjectileGravityScale = 0.0f; // 기본 무중력
	MovementComponent->bSweepCollision = false; // [최적화 핵심] 이거 끄세요!

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(RootComponent); // 충돌체에 딱 달라붙어라
	NiagaraComponent->SetAutoActivate(true);          // 생성되자마자 이펙트 재생해라
	NiagaraComponent->SetRelativeLocation(FVector::ZeroVector); // 위치는 정중앙


	// 4. [기본 변수 초기화]
	Penetration = 1; // 기본 1
	Damage = 0.0f;
	LifeTime = 3.0f;
 
}

// Called when the game starts or when spawned
void ADESimpleProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADESimpleProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LifeTimeCounter += DeltaTime;
	if (LifeTimeCounter >= LifeTime)
	{
		OnLifeTimeExpired();
		return;
	}

	// [최적화] 가속도가 있을 때만 연산
	// KINDA_SMALL_NUMBER (0.0001) 체크도 비용이다. 그냥 0.0f 비교가 더 빠를 수도 있음.
	if (MovementComponent && Acceleration != 0.0f)
	{
		// 1. float 덧셈 (CPU가 제일 좋아하는 연산)
		CurrentSpeed += (Acceleration * DeltaTime);

		// 2. 음수 방지 & MaxSpeed 제한
		if (CurrentSpeed < 0.0f) CurrentSpeed = 0.0f;
		// if (CurrentSpeed > MaxSpeed) ... (필요하면)

		// 3. [핵심] 제곱근 연산(sqrt) 없이 단순 곱셈으로 속도 적용
		MovementComponent->Velocity = ShootDirection * CurrentSpeed;
	}

	//zis old sqrt logic = no good :/
	//// 2. [가속/감속 로직]
	//if (MovementComponent && Acceleration != 0.0f) // 가속도가 0이 아닐 때만 계산 (성능 절약)
	//{
	//	// 현재 속도 크기 가져오기
	//	float CurrentSpeed = MovementComponent->Velocity.Size();

	//	// 새 속도 계산 (현재 속도 + 가속도 * 시간)
	//	float NewSpeed = CurrentSpeed + (Acceleration * DeltaTime);

	//	// [중요] 감속일 때 속도가 음수가 되면 뒤로 날아갑니다. 
	//	// 멈추게 하고 싶다면 0.0f로 막아줘야 합니다.
	//	if (NewSpeed < 0.0f)
	//	{
	//		NewSpeed = 0.0f;
	//	}

	//	// [중요] 가속일 때 MaxSpeed를 뚫고 싶다면 MaxSpeed도 같이 늘려줘야 합니다.
	//	if (NewSpeed > MovementComponent->MaxSpeed)
	//	{
	//		MovementComponent->MaxSpeed = NewSpeed;
	//	}

	//	// 방향은 유지하고(GetSafeNormal), 속력(Speed)만 갈아끼움
	//	// Velocity가 0일 때 GetSafeNormal()하면 (0,0,0) 나와서 안전함
	//	if (MovementComponent->Velocity.SizeSquared() > KINDA_SMALL_NUMBER) // 움직이고 있을 때만
	//	{
	//		MovementComponent->Velocity = MovementComponent->Velocity.GetSafeNormal() * NewSpeed;
	//	}
	//}
}

void ADESimpleProjectileBase::ResetState()
{
	//zis for init only
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	HitActors.Empty();
	LifeTimeCounter = 0.f;

	// 2. [중요] 충돌 다시 켜기
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// 3. [중요] 나이아가라 강제 재시작 (아까 질문하신 해결책!)
	if (NiagaraComponent)
	{
		NiagaraComponent->SetVisibility(true);
		NiagaraComponent->Activate(true); // true = Reset해서 처음부터 다시 재생
	}

	// 4. 무브먼트 컴포넌트 활성화
	if (MovementComponent)
	{
		MovementComponent->Activate();
		MovementComponent->SetUpdatedComponent(CollisionComponent); // 혹시 연결 끊겼을까봐
	}
}

void ADESimpleProjectileBase::ReturnToPool()
{
	// 1. [중요] 나이아가라 끄기 (안 끄면 풀 안에서 계속 번쩍거림)
	if (NiagaraComponent)
	{
		NiagaraComponent->Deactivate();
		NiagaraComponent->SetVisibility(false);
	}

	// 2. [중요] 충돌 끄기 (안 끄면 풀 안에서 몬스터 때림)
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 3. 무브먼트 멈추기
	if (MovementComponent)
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->Deactivate();
	}

	if (UWorld* World = GetWorld())
	{
		if (UGameInstance* GI = World->GetGameInstance())
		{
			if (auto* Pool = World->GetSubsystem<UDEPoolSubsystem>())
			{
				//ResetState();
				Pool->ReturnActorToPool(this);
			}
		}
	}
}

void ADESimpleProjectileBase::InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction)
{
	

	Damage = InDamage;
	ShootDirection = Direction.GetSafeNormal(); // 처음에 딱 한 번만 정규화
	CurrentSpeed = (InSpeed == 0.0f) ? 1200.0f : InSpeed;
	Penetration = InPenetration;

	MovementComponent->Velocity = ShootDirection * CurrentSpeed;

	ResetState();
}

void ADESimpleProjectileBase::InitializeFromContext(const FDESkillContext& Context, const FVector& Direction)
{
	// 1. 멤버 변수에 저장 (캐싱)
	if (APawn* InstigatorPawn = Cast<APawn>(Context.Instigator))
	{
		SetInstigator(InstigatorPawn);
	}
	Snapshot = Context.FinalSnapshot;
	Damage = Context.Damage;             // ★ 중요: 곱하기 하지 마세요! 순수 데미지 저장
	CritChance = Context.CritChance;
	CritDamageMultiplier = Context.CritDamageMultiplier; // ★ 저장
	KnockbackForce = Context.KnockbackForce;
	Penetration = Context.Penetration;
	Speed = Context.Speed;
	LifeTime = Context.Duration;         // Duration을 LifeTime으로 매핑
	EffectRadius = Context.Radius;

	// 2. 물리적 초기화
	// (InitializeProjectile 함수 내부도 멤버 변수 쓰는 걸로 바꾸거나, 여기 있는 값을 넘김)
	InitializeProjectile(Damage, Speed, Penetration, Direction);

	// 3. 크기 조절 (옵션)
	float NewSize = Context.GetValue(TEXT("Size"), -1.f);
	if (NewSize > 0.f)
	{
		SetSize(NewSize);
		// 여기서 실제 Mesh 스케일 조절 로직 호출
	}
	/*if (EffectRadius > 0.0f)
	{
		SetSize(EffectRadius);
	}*/


}

void ADESimpleProjectileBase::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. [유효성 검사]
	if (!OtherActor || OtherActor == this || OtherActor == GetInstigator() || HitActors.Contains(OtherActor)) return;

	// 2. [라이브러리 호출]
	// 굳이 여기서 TargetHealth를 Find할 필요도 없습니다. 라이브러리가 대신 해줄 테니까요.
	HitActors.Add(OtherActor);

	// 넉백 방향 계산
	FVector KBDir = OtherActor->GetActorLocation() - GetActorLocation();

	// ★ [핵심] 주문서(Request) 작성
	FDEDamageRequest Req;
	Req.Instigator = GetInstigator();
	Req.DamageCauser = this;
	Req.Victim = OtherActor; // 맞은 놈을 넣어줍니다.
	Req.BaseDamage = Damage;
	Req.CritChance = CritChance; // 멤버 변수로 들고 있는 Snapshot 활용
	Req.CritDamageMultiplier = CritDamageMultiplier;

	// 라이브러리에 던지기 (피흡, 넉백, 킬 처리가 한 방에 끝남)
	//UE_LOG(LogTemp, Log, TEXT("Try DEGameplayLibrary"));
	FDEDamageResult Res = UDEGameplayLibrary::ApplyCombatDamage(Req, this->Snapshot, KBDir, this->KnockbackForce);

	// 3. [관통 로직만 투사체 본연의 업무로 남김]
	if (Res.FinalDamage > 0.0f && Penetration != -1)
	{
		if (--Penetration <= 0) ReturnToPool();
	}

	//// 1. [유효성 검사]
	//// - 대상이 없거나
	//// - 나 자신이거나 (투사체끼리 충돌 방지)
	//// - 나를 쏜 주인(플레이어)이거나
	//// - 이미 맞은 대상이라면 (샷건처럼 한 틱에 여러 번 맞는 것 방지)
	//if (!OtherActor || OtherActor == this || OtherActor == GetInstigator() || HitActors.Contains(OtherActor))
	//{
	//	return;
	//}

	//// 2. [대상 확인] 체력 컴포넌트가 있는가?
	//// (꼭 몬스터가 아니더라도, 파괴 가능한 오브젝트 등 HP가 있는 모든 것을 때릴 수 있음)
	//UDEHealthComponent* TargetHealth = OtherActor->FindComponentByClass<UDEHealthComponent>();

	//if (TargetHealth)
	//{
	//	// 중복 피격 방지 목록에 등록
	//	HitActors.Add(OtherActor);

	//	// ====================================================
	//	// [AAA 스타일] 1. 데미지 신청서(Request) 작성
	//	// ====================================================
	//	// 투사체는 계산을 하지 않습니다. "내 스펙은 이렇습니다"라고 적어서 냅니다.
	//	FDEDamageRequest Req;
	//	Req.Instigator = GetInstigator();     // 때린 사람 (플레이어)
	//	Req.DamageCauser = this;            // 때린 도구 (투사체)

	//	// ★ 중요: Context가 아니라 '내 멤버 변수'에 저장된 값을 사용합니다.
	//	Req.BaseDamage = this->Damage;           // 기본 깡뎀
	//	Req.CritChance = this->CritChance;       // 치명타 확률
	//	Req.CritDamageMultiplier = this->CritDamageMultiplier; // 치명타 배율 (예: 2.0)

	//	// ====================================================
	//	// [AAA 스타일] 2. 처리 요청 및 결과 수신
	//	// ====================================================
	//	// "계산은 님이(HealthComponent) 하시고 결과만 알려주세요."
	//	// 여기서 상대방의 방어력, 무적, 회피 등이 모두 계산됩니다.
	//	FDEDamageResult Res = TargetHealth->ProcessDamage(Req);


	//	// ====================================================
	//	// [AAA 스타일] 3. 피드백 루프 (결과에 따른 행동)
	//	// ====================================================

	//	// 3-1. 넉백 적용 (몬스터인 경우에만)
	//	// 몬스터가 '슈퍼아머' 상태라면 ApplyKnockback 내부에서 무시하도록 설계하면 됨.
	//	if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(OtherActor))
	//	{
	//		// 방향 계산: 몬스터 위치 - 투사체 위치 = 밀려나는 방향
	//		FVector KnockbackDir = Monster->GetActorLocation() - GetActorLocation();
	//		//KnockbackDir.Z = 0.0f; // 공중으로 뜨지 않게 평면 고정
	//		//KnockbackDir.Normalize();

	//		// 내 멤버 변수(KnockbackForce) 사용
	//		Monster->ApplyKnockback(KnockbackDir,this->KnockbackForce);
	//	}

	//	// 3-2. 흡혈 (Life Steal) 로직 예시
	//	// 실제로 데미지가 들어갔을 때만 흡혈
	//	if (Res.FinalDamage > 0.0f)
	//	{
	//		// TODO: 플레이어 스탯 컴포넌트에 회복 요청
	//		// PlayerStats->Heal(Res.FinalDamage * 0.1f); 
	//	}

	//	// 3-3. 처치 시 효과 (On Kill Effect)
	//	if (Res.bIsDead)
	//	{
	//		// TODO: 킬 스택 쌓기, 경험치 구슬 추가 드롭 등
	//		// UE_LOG(LogTemp, Log, TEXT("Target Eliminated!"));
	//	}

	//	// 4. [관통 로직]
	//	// Penetration이 -1이면 무한 관통
	//	if (Penetration != -1)
	//	{
	//		--Penetration;
	//		if (Penetration <= 0)
	//		{
	//			ReturnToPool(); // 관통 횟수 소진 시 풀로 반환
	//		}
	//	}
	//}
	//else
	//{
	//	// (선택 사항) 체력이 없는 벽이나 장애물에 맞았을 때
	//	// 보통 뱀서류에서는 벽에 맞으면 사라지거나 튕김
	//	// ReturnToPool(); 
	//}
}
void ADESimpleProjectileBase::OnLifeTimeExpired()
{
	ReturnToPool();
}

void ADESimpleProjectileBase::SetSize(float NewSize)
{
	// 1. 데이터 갱신
	Size = NewSize;

	// 2. 스케일 계산 (기본 반지름을 30.0f라고 가정했을 때 비율 계산)
	// 만약 기본 반지름이 30인데 33이 들어오면 -> 1.1배 커짐 (+10%)
	const float DefaultRadius = 30.0f;
	float NewScale = NewSize / DefaultRadius;

	// 3. 액터 전체 크기 변경 (충돌체 + 나이아가라 모두 커짐)
	// 이렇게 하면 따로 나이아가라 파라미터를 건드릴 필요 없이 다 같이 커집니다.
	SetActorScale3D(FVector(NewScale));

	// (참고) 만약 스케일 말고 충돌체만 정밀하게 조절하고 싶다면 아래 코드를 씁니다.
	// 하지만 뱀서류는 그냥 SetActorScale3D 쓰는 게 성능도 좋고 관리도 편합니다.
	/*
	if (CollisionComponent)
	{
		CollisionComponent->SetSphereRadius(Size);
	}
	*/
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