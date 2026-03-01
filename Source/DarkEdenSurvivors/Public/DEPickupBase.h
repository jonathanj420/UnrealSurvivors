// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DEPickupBase.generated.h"


class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class DARKEDENSURVIVORS_API ADEPickupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADEPickupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

public:
	// =========================================================
	// [Pool Interface] 풀링 & 초기화
	// =========================================================

	// 풀에서 꺼낼 때 호출 (위치 지정 + 활성화)
	virtual void ActivatePickup(const FVector& Location, float DataValue = 0.0f);

	// 풀로 돌아갈 때 호출 (비활성화)
	virtual void DeactivatePickup();

	// =========================================================
	// [Magnet System] 자석 기능
	// =========================================================

	//// 외부(플레이어)에서 호출: "나한테 빨려와!"
	//void StartMagnetMovement(AActor* TargetActor);

protected:
	// =========================================================
	// [Effect] 자식들이 구현할 내용
	// =========================================================

	// 실제 효과 적용 (경험치 주기, 체력 회복 등)
	// TargetActor: 먹은 사람 (플레이어)
	

protected:
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

public:

	virtual void ApplyEffect(AActor* TargetActor);
	// 자석 이동 관련
	bool bIsMagnetized = false;
	bool bCanBeMagnetized = false;
	UPROPERTY()
	AActor* MagnetTarget = nullptr;

	float CurrentSpeed = 0.0f;
	float Acceleration = 2500.0f; // 빨려가는 가속도

	// 자식들이 쓸 공용 데이터 (경험치 양, 회복량 등)
	float Value = 0.0f;
	void SetTriggerRadius(float InSize);

};
