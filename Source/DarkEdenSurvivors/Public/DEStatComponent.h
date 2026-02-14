// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEStatTypes.h"
#include "DEStatComponent.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnLevelUp);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpeedChanged, float /*NewSpeed*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMagnetChanged, float /*NewRadius*/);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDEStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// NEw
public:
	// =========================================================
	// [1] Physical Stats (신체 능력)
	// =========================================================

	// 이동 속도 (기본값 600)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat MoveSpeed;

	// 자석 범위 (아이템 획득 반경)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Physical")
	FGameplayStat MagnetRange;

	// =========================================================
	// [2] Utility Stats (유틸리티/파밍)
	// =========================================================

	// 행운 (Luck): 치명타 확률 보정, 좋은 아이템 드랍 확률 등
	// 기본 1.0 (100%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Utility")
	FGameplayStat Luck;

	// 탐욕 (Greed): 골드 획득량 배율
	// 기본 1.0 (100%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Utility")
	FGameplayStat Greed;

	// 저주 (Curse): 적의 강력함, 스폰량 증가 (리스크 앤 리턴)
	// 기본 1.0 (100%) -> 높을수록 어려워짐
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats | Utility")
	FGameplayStat Curse;

	// =========================================================
	// [3] Functions (기능)
	// =========================================================

	// 스탯이 변경되었을 때 호출하여 실제 게임에 적용 (아이템 획득 시 호출)
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void UpdateMovementSpeed();

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void UpdateMagnetRange();

	// 풀링(Pooling)을 위한 초기화
	void ResetStats();

	// Getters (값만 필요할 때)
	float GetMoveSpeed() const { return MoveSpeed.GetValue(); }
	float GetMagnetRange() const { return MagnetRange.GetValue(); }
	float GetLuck() const { return Luck.GetValue(); }
	float GetGreed() const { return Greed.GetValue(); }
	float GetCurse() const { return Curse.GetValue(); }

public:
	// 변경 알림 이벤트
	FOnSpeedChanged OnSpeedChanged;
	FOnMagnetChanged OnMagnetChanged;
		
};
