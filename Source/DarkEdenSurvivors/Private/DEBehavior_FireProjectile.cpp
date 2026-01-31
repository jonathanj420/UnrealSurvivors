// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_FireProjectile.h"
#include "DEPoolSubsystem.h"
#include "DESimpleProjectileBase.h" // 투사체 헤더
#include "Kismet/GameplayStatics.h"


void UDEBehavior_FireProjectile::Execute(FDESkillContext& Context)
{
	//UE_LOG(LogTemp, Warning, TEXT("Behavior : FireProjectile Excuted"));
	if (!Context.Instigator) return;

	int32 Count = FMath::Max(1, Context.ProjectileCount);

	// [Case A: 동시 발사 (기존)]
	if (BurstInterval <= 0.f)
	{
		// Context를 복사할 필요 없이 그냥 반복문으로 쏨
		// (Execute 함수 안에서 잠시 쓸 용도로 CachedContext 활용)
		CachedContext = Context;
		for (int32 i = 0; i < Count; i++)
		{
			FireOneShot(); // 그냥 연속 호출
		}
		return;
	}

	// [Case B: 연사 (Delay)]
	// 타이머는 나중에 실행되므로 Context를 백업해둬야 함!
	CachedContext = Context;
	RemainingShots = Count;
	//UE_LOG(LogTemp, Warning, TEXT("Fire Projectile : Remaining Shots : %d"), RemainingShots);
	UWorld* World = Context.Instigator->GetWorld();
	if (World)
	{
		// 첫 발은 즉시 발사
		FireOneShot();
		//RemainingShots--;

		if (RemainingShots > 0)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Remaining Shots Left: %d, BURST ACTIVATED"), RemainingShots);
			// 나머지는 타이머로
			World->GetTimerManager().SetTimer(
				BurstTimerHandle,
				this,
				&UDEBehavior_FireProjectile::FireOneShot,
				BurstInterval,
				true // 반복
			);
		}
	}
}

void UDEBehavior_FireProjectile::FireOneShot()
{
	// Context 대신 백업해둔 CachedContext 사용
	if (!CachedContext.Instigator || !ProjectileClass)
	{
		// 안전장치: 시전자가 죽거나 사라졌으면 타이머 정지
		if (CachedContext.Instigator)
		{
			CachedContext.Instigator->GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
		}
		return;
	}

	UWorld* World = CachedContext.Instigator->GetWorld();
	if (!World) return;

	UDEPoolSubsystem* Pool = World->GetGameInstance()->GetSubsystem<UDEPoolSubsystem>();
	if (!Pool) return;

	// --- 발사 로직 (동일) ---
	FVector Forward = CachedContext.Instigator->GetActorForwardVector();
	FVector Right = CachedContext.Instigator->GetActorRightVector();
	FVector Up = CachedContext.Instigator->GetActorUpVector();
	FVector OwnerLoc = CachedContext.Instigator->GetActorLocation();
	FRotator OwnerRot = CachedContext.Instigator->GetActorRotation();

	// 1. 위치 랜덤
	float RandRight = FMath::FRandRange(-RandomPositionRange, RandomPositionRange);
	float RandUp = FMath::FRandRange(-RandomPositionRange, RandomPositionRange);
	FVector SpawnLoc = OwnerLoc + (Forward * 100.f) + (Right * RandRight) + (Up * RandUp);

	// 2. 각도 랜덤
	FRotator SpawnRot = OwnerRot;
	if (FireConeAngle > 0.f)
	{
		float HalfAngle = FireConeAngle * 0.5f;

		float RandYaw = FMath::FRandRange(-HalfAngle, HalfAngle);
		SpawnRot.Yaw += RandYaw;
	}

	// 3. 스폰
	AActor* PooledActor = Pool->GetPooledActor(ProjectileClass, SpawnLoc, SpawnRot, true);
	if (auto* Proj = Cast<ADESimpleProjectileBase>(PooledActor))
	{
		FVector FireDir = SpawnRot.Vector();
		Proj->InitializeFromContext(CachedContext, FireDir);
	}

	// 4. 사운드
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(World, FireSound, SpawnLoc);
	}

	//// [연사 모드일 때만 종료 체크]
	//if (BurstInterval > 0.f)
	//{
	//	RemainingShots--;
	//	if (RemainingShots <= 0)
	//	{
	//		World->GetTimerManager().ClearTimer(BurstTimerHandle);
	//	}
	//}
	RemainingShots--;
	if (RemainingShots <= 0)
	{
		World->GetTimerManager().ClearTimer(BurstTimerHandle);

	}
}