// Fill out your copyright notice in the Description page of Project Settings.


#include "DEProgressionComponent.h"
#include "DEStatComponent.h"

// Sets default values for this component's properties
UDEProgressionComponent::UDEProgressionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UDEProgressionComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentLevel = 1;
	CurrentExp = 0.0f;
	CalculateNextLevelExp();

	// 시작 시 UI 초기화
	OnExpChanged.Broadcast(CurrentExp, MaxExp);
	// ...
	
	// ★ 게임 시작(생성) 시점에 딱 한 번만 찾아서 포인터를 쥐고 있는다!
	if (AActor* Owner = GetOwner())
	{
		CachedStatComp = Owner->FindComponentByClass<UDEStatComponent>();
	}

}



void UDEProgressionComponent::CalculateNextLevelExp()
{
	int32 RequiredVal = 0;

	// [Phase 1] Lv 1 ~ 19
	if (CurrentLevel < 20)
	{
		RequiredVal = BaseXP_Lv1 + (CurrentLevel - 1) * Inc_Lv1;
	}
	// [Phase 2] Lv 20 ~ 39
	else if (CurrentLevel < 40)
	{
		int32 BaseXP_Lv20 = BaseXP_Lv1 + (19 * Inc_Lv1);
		RequiredVal = BaseXP_Lv20 + (CurrentLevel - 20) * Inc_Lv21;
	}
	// [Phase 3] Lv 40 ~
	else
	{
		int32 BaseXP_Lv20 = BaseXP_Lv1 + (19 * Inc_Lv1);
		int32 BaseXP_Lv40 = BaseXP_Lv20 + (20 * Inc_Lv21);
		RequiredVal = BaseXP_Lv40 + (CurrentLevel - 40) * Inc_Lv41;
	}

	// [성장 벽] Lv 20, 40일 때 요구량 폭증
	if (CurrentLevel == 20) RequiredVal += WallXP_Lv20;
	if (CurrentLevel == 40) RequiredVal += WallXP_Lv40;

	MaxExp = (float)RequiredVal;


	//older
	//// 단순 공식: 이전 통 * 1.2배
	//// 기획에 따라 엑셀 데이터 테이블을 읽어오는 방식 등으로 변경 가능
	//if (CurrentLevel == 1)
	//{
	//	MaxExp = BaseExpRequirement;
	//}
	//else
	//{
	//	MaxExp = MaxExp * ExpGrowthFactor;
	//}
}

void UDEProgressionComponent::LevelUp()
{
	// 치트용: 경험치 시스템 무시하고 레벨만 +1
	CurrentLevel++;
	CurrentExp = 0.f;
	CalculateNextLevelExp();
	OnLevelUp.Broadcast(CurrentLevel);
	UE_LOG(LogTemp, Warning, TEXT("Level Up! New Level: %d"), CurrentLevel);
}
void UDEProgressionComponent::AddExp(float Amount)
{
	// [성장 벽 버프] 20, 40레벨일 때 획득량 2배 (지루함 방지)
	if (CurrentLevel == 20 || CurrentLevel == 40)
	{
		Amount *= 2.0f;
	}

	float FinalExp = Amount;

	// [StatComponent 연동] (님이 짜신 코드 유지! 아주 좋습니다)
	if (CachedStatComp)
	{
		FinalExp *= CachedStatComp->GetGrowth();
		//if (UDEStatComponent* StatComp = Owner->FindComponentByClass<UDEStatComponent>())
		//{
		//	// StatComponent에 GetGrowth() 함수가 있다고 가정
		//	// (없으면 StatComp->GrowthStat.GetValue() 등으로 수정하세요)
		//	 FinalExp *= CachedStatComp->GetGrowth();
		//}
	}

	// 경험치 적용
	CurrentExp += FinalExp;

	// [레벨업 루프] 한 번에 여러 레벨업 가능하게 처리
	while (CurrentExp >= MaxExp)
	{
		CurrentExp -= MaxExp; // 남은 경험치 이월
		CurrentLevel++;

		// 레벨업 했으니 다음 통 계산
		CalculateNextLevelExp();

		// 알림
		OnLevelUp.Broadcast(CurrentLevel);
		UE_LOG(LogTemp, Warning, TEXT("Level Up! New Level: %d"), CurrentLevel);
	}

	// UI 갱신 (Safety Check 추가)
	float Percent = (MaxExp > KINDA_SMALL_NUMBER) ? FMath::Clamp(CurrentExp / MaxExp, 0.0f, 1.0f) : 0.0f;
	OnExpChanged.Broadcast(CurrentExp, MaxExp);
}