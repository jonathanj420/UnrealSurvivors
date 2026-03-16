// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPickupManager.h"
#include "DEPickupBase.h"
#include "DEPoolSubsystem.h"
#include "DEStatComponent.h"
#include "Kismet/GameplayStatics.h"   
//#include "Items/DEPotion.h"      
//#include "Items/DEMagnetItem.h"
UDEPickupManager::UDEPickupManager()
{

}

void UDEPickupManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 초기화
	ActivePickups.Empty();
	MagnetizingPickups.Empty();
	CachedStatComponent = nullptr;
}

void UDEPickupManager::Deinitialize()
{
	ActivePickups.Empty();
	MagnetizingPickups.Empty();
	CachedStatComponent = nullptr;
	Super::Deinitialize();
}

void UDEPickupManager::SpawnPickup(const FVector& Location, float ItemValue, TSubclassOf<ADEPickupBase> ClassToSpawn)
{
	if (!ClassToSpawn || ActivePickups.Num() >= MaxActivePickups) return;

	UDEPoolSubsystem* Pool = GetWorld()->GetSubsystem<UDEPoolSubsystem>();
	if (!Pool) return;

	AActor* SpawnedActor = Pool->GetPooledActor(ClassToSpawn, Location, FRotator::ZeroRotator, false);

	if (ADEPickupBase* Pickup = Cast<ADEPickupBase>(SpawnedActor))
	{
		// ★ 구조체에서 가져온 Value를 픽업 아이템에게 주입!
		Pickup->ActivatePickup(Location, ItemValue);
		ActivePickups.Add(Pickup);
	}
}

void UDEPickupManager::Tick(float DeltaTime)
{
	if (GetWorld() && GetWorld()->IsPaused())
	{
		return;
	}

	// -------------------------------------------------------------
	// 0. 플레이어 & 스탯 컴포넌트 캐싱 (없으면 찾기)
	// -------------------------------------------------------------
	if (!CachedStatComponent.IsValid())
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			CachedStatComponent = PlayerPawn->FindComponentByClass<UDEStatComponent>();
		}
	}

	// 플레이어가 없거나 죽었으면 로직 중단
	if (!CachedStatComponent.IsValid()) return;

	AActor* PlayerActor = CachedStatComponent->GetOwner();
	if (!PlayerActor) return;


	// -------------------------------------------------------------
	// [1단계] 레이더 가동: "내 자석 범위 안에 들어온 놈 있나?"
	// -------------------------------------------------------------

	// ★ 스탯 컴포넌트에서 범위 가져오기
	// (구조체 멤버 이름이 Value라면 .Value, 함수라면 .GetValue()로 수정하세요)
	float CurrentRange = CachedStatComponent->GetMagnetRange();
	float PickupRadiusSq = CurrentRange * CurrentRange; // 제곱 거리 사용

	FVector PlayerLoc = PlayerActor->GetActorLocation();

	// 전체 아이템 스캔 (RemoveAtSwap을 위해 역순)
	for (int32 i = ActivePickups.Num() - 1; i >= 0; --i)
	{
		if (!ActivePickups[i].IsValid())
		{
			ActivePickups.RemoveAtSwap(i);
			continue;
		}

		ADEPickupBase* Pickup = ActivePickups[i].Get();

		// 이미 빨려오는 중이거나, 숨겨진(먹힌) 놈은 패스
		if (Pickup->bIsMagnetized || Pickup->IsHidden()) continue;

		// 거리 체크
		float DistSq = FVector::DistSquared(Pickup->GetActorLocation(), PlayerLoc);

		// 범위 안에 들어왔다! -> 자석 리스트 등록
		if (DistSq < PickupRadiusSq)
		{
			RegisterMagnetRequest(Pickup, PlayerActor);

		}
	}


	// -------------------------------------------------------------
	// [2단계] 자석 이동: "빨려오기로 한 놈들 실제로 옮기기"
	// -------------------------------------------------------------

	for (int32 i = MagnetizingPickups.Num() - 1; i >= 0; --i)
	{
		if (!MagnetizingPickups[i].IsValid())
		{
			MagnetizingPickups.RemoveAtSwap(i);
			continue;
		}

		ADEPickupBase* Pickup = MagnetizingPickups[i].Get();

		// 타겟이 사라졌거나, (충돌 이벤트에 의해) 이미 먹혀서 숨겨졌다면 리스트에서 아웃!
		if (!Pickup || !Pickup->MagnetTarget || Pickup->IsHidden())
		{
			MagnetizingPickups.RemoveAtSwap(i);
			continue;
		}

		// 이동 로직 (매니저는 배달만 한다!)
		FVector MyLoc = Pickup->GetActorLocation();
		FVector TargetLoc = Pickup->MagnetTarget->GetActorLocation();
		TargetLoc.Z += 50.0f; // 허리 높이

		// 방향 구하고 가속해서 이동
		FVector Dir = (TargetLoc - MyLoc).GetSafeNormal();
		Pickup->CurrentSpeed += (2500.0f * DeltaTime);
		Pickup->SetActorLocation(MyLoc + (Dir * Pickup->CurrentSpeed * DeltaTime));

	}
}

void UDEPickupManager::TriggerGlobalMagnet(AActor* TargetPlayer)
{
	if (!TargetPlayer) return;

	for (int32 i = ActivePickups.Num() - 1; i >= 0; --i)
	{
		if (ActivePickups[i].IsValid())
		{
			ADEPickupBase* Pickup = ActivePickups[i].Get();

			if (Pickup && !Pickup->IsHidden() && Pickup->bCanBeMagnetized)
			{
				RegisterMagnetRequest(Pickup, TargetPlayer);
			}
		}
	}
}

void UDEPickupManager::RegisterMagnetRequest(ADEPickupBase* Pickup, AActor* Target)
{
	if (Pickup && Target && !Pickup->bIsMagnetized)
	{
		Pickup->MagnetTarget = Target;
		Pickup->bIsMagnetized = true;
		Pickup->CurrentSpeed = 300.0f; // 초기 속도 (살짝 튀어오름)

		// 이동 처리 목록에 추가
		MagnetizingPickups.Add(Pickup);
	}
}

void UDEPickupManager::ClearAllPickups()
{
	// 모든 아이템 강제 반환
	for (auto& WeakPickup : ActivePickups)
	{
		if (WeakPickup.IsValid())
		{
			WeakPickup.Get()->DeactivatePickup();
		}
	}
	ActivePickups.Empty();
	MagnetizingPickups.Empty();
}