// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_FireProjectileRadial.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "DESkillContext.h"
#include "DEPoolSubsystem.h"
#include "DESimpleProjectileBase.h"


void UDEBehavior_FireProjectileRadial::Execute(FDESkillContext& Context)
{
	// 1. 필수 데이터 검사
	if (!Context.Instigator || !ProjectileClass) return;

	UWorld* World = Context.Instigator->GetWorld();
	if (!World) return;

	UDEPoolSubsystem* Pool = World->GetSubsystem<UDEPoolSubsystem>();
	if (!Pool) return;

	// 2. 발사 원점 설정
	FVector Origin = Context.Instigator->GetActorLocation();

	// 3. 기준 회전(BaseRot) 설정
	// 기본값: 플레이어가 보는 방향
	FRotator BaseRot = Context.Instigator->GetActorRotation();

	// ★ [핵심] 타겟이 있다면, 타겟을 바라보는 방향으로 기준점 변경
	if (Context.Targets.Num() > 0 && Context.Targets[0])
	{
		FVector TargetLoc = Context.Targets[0]->GetActorLocation();
		// 내 위치에서 타겟 위치를 바라보는 회전값 계산
		BaseRot = UKismetMathLibrary::FindLookAtRotation(Origin, TargetLoc);
	}

	// 4. 각도 계산
	int32 Count = FMath::Max(1, Context.Amount);

	// 전체 각도(ArcAngle)를 개수로 나눔 (예: 360 / 3 = 120도)
	float AngleStep = (Count > 0) ? (ArcAngle / (float)Count) : 0.f;

	// (옵션) 만약 360도가 아니라 부채꼴(120도 등)이라면, 
	// 타겟을 '중심'에 두기 위해 시작 각도를 절반만큼 뒤로 감아줌.
	// 360도일 때는 굳이 안 해도 티가 안 나지만, 부채꼴일 땐 이게 있어야 자연스러움.
	// ex) 120도짜리 케익을 3조각해서 케익 끝 중앙에 피크를 꽂는게 이 예시
	float StartYawOffset = 0.f;
	if (FMath::IsNearlyEqual(ArcAngle, 360.f) == false)
	{
		// 예: 120도 부채꼴이면 -60도부터 시작해서 타겟을 가운데 둠
		StartYawOffset = -ArcAngle * 0.5f + (AngleStep * 0.5f);
	}


	// 5. 발사 루프
	for (int32 i = 0; i < Count; i++)
	{
		// 기준 회전(BaseRot) 복사
		FRotator SpawnRot = BaseRot;

		// Yaw(수평 회전) 적용
		float CurrentYawStep = StartYawOffset + (AngleStep * i);
		SpawnRot.Yaw += CurrentYawStep;

		FVector FireDir = SpawnRot.Vector();

		// 풀에서 꺼내기
		AActor* PooledActor = Pool->GetPooledActor(
			ProjectileClass,
			Origin,
			SpawnRot,
			true
		);

		// 초기화
		if (auto* Proj = Cast<ADESimpleProjectileBase>(PooledActor))
		{
			Proj->InitializeFromContext(Context, FireDir);
		}
	}

	// 6. 사운드 재생 (한 번만)
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(World, FireSound, Origin);
	}


}
