// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEProgressionComponent.generated.h"

// [델리게이트]
// 경험치 변경 알림 (UI 갱신용: 0.0~1.0 퍼센트, 현재값, 최대값)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnExpChanged, float, float);
// 레벨업 알림 (스킬 선택창 팝업용)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEProgressionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDEProgressionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(Transient) // 세이브 파일에 저장할 필요 없는 임시 데이터라는 뜻
		class UDEStatComponent* CachedStatComp;

public:
	// 경험치 획득 (외부에서 호출: 보석 먹었을 때)
	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddExp(float Amount);

	// 현재 레벨 확인
	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetCurrentLevel() const { return CurrentLevel; }
	float GetMaxExp() const { return MaxExp; }
	float GetCurrentExp() const { return CurrentExp; }


public:
	// UI 바인딩용 델리게이트
	FOnExpChanged OnExpChanged;

	FOnLevelUp OnLevelUp;

protected:
	// 경험치통 계산 로직
	void CalculateNextLevelExp();

private:
	// 내부 변수들
	UPROPERTY(VisibleAnywhere, Category = "Progression")
	int32 CurrentLevel = 1;

	UPROPERTY(VisibleAnywhere, Category = "Progression")
	float CurrentExp = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Progression")
	float MaxExp = 100.0f;


	//maybe later use?
	// [밸런스] 레벨업 계수
	//UPROPERTY(EditDefaultsOnly, Category = "Balance")
	//float BaseEXPRequirement = 100.0f;

	//UPROPERTY(EditDefaultsOnly, Category = "Balance")
	//float EXPGrowthFactor = 1.2f; // 레벨당 필요 경험치 20% 증가
	
	//vampsurv original exp calc
	// [Phase 1: Lv 1 ~ 20]
	UPROPERTY(EditDefaultsOnly, Category = "Balance | VS Style")
	int32 BaseXP_Lv1 = 5;       // 1레벨 기본 요구량

	UPROPERTY(EditDefaultsOnly, Category = "Balance | VS Style")
	int32 Inc_Lv1 = 10;         // 레벨당 증가량 (10)

	// [Phase 2: Lv 21 ~ 40]
	UPROPERTY(EditDefaultsOnly, Category = "Balance | VS Style")
	int32 Inc_Lv21 = 13;        // 21부터 증가량 (13)

	// [Phase 3: Lv 41 ~ ]
	UPROPERTY(EditDefaultsOnly, Category = "Balance | VS Style")
	int32 Inc_Lv41 = 16;        // 41부터 증가량 (16)

	// [Growth Walls: 성장 벽]
	// 특정 레벨에서 요구량을 폭증시켜 성장을 늦추는 구간
	UPROPERTY(EditDefaultsOnly, Category = "Balance | VS Style")
	int32 WallXP_Lv20 = 600;    // 20레벨 벽 (+600)

	UPROPERTY(EditDefaultsOnly, Category = "Balance | VS Style")
	int32 WallXP_Lv40 = 2400;   // 40레벨 벽 (+2400)

		
public:
	void LevelUp();

};
