// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatComponent.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Engine/Engine.h"

// Sets default values for this component's properties
UDEStatComponent::UDEStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDEStatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;
	
}


// Called every frame
void UDEStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsPlayer)
	{
		if (GEngine)
		{
			// 1. HP 정보 출력
			FString HP_Msg = FString::Printf(
				TEXT("[HP] %s: %.1f / %.1f"),
				*GetOwner()->GetName(), // 몬스터 이름/플레이어 이름
				CurrentHP,
				MaxHP
			);
			GEngine->AddOnScreenDebugMessage(
				1, // Key 1번: HP 전용 위치
				-1.0f, // TimeToDisplay: -1.0f는 다음 프레임에 갱신될 때까지 유지 (Tick에서 사용 시 효과적)
				FColor::Cyan,
				HP_Msg
			);

			// 2. EXP 정보 출력 (플레이어에게만 해당된다면 조건문으로 감싸야 합니다.)
			FString EXP_Msg = FString::Printf(
				TEXT("[EXP] LV %d: %.0f / %.0f"),
				Level,
				CurrentEXP,
				NextLevelEXP
			);
			GEngine->AddOnScreenDebugMessage(
				2, // Key 2번: EXP 전용 위치 (HP 밑에 나옴)
				-1.0f,
				FColor::Yellow,
				EXP_Msg
			);
		}
	}
	// ...
	
}


void UDEStatComponent::EndInvincible()
{
	bInvincible = false;
}

void UDEStatComponent::ResetStat()
{
	CurrentHP = MaxHP;
	bInvincible = false;
	// 필요한 초기화들 추가
}

float UDEStatComponent::TakeDamage(float DamageAmount, AActor* DamageCauser)
{
	
	if (DamageAmount <= 0.f) return 0.f;

	CurrentHP = FMath::Clamp(CurrentHP - DamageAmount, 0.f, MaxHP);
	OnHPChanged.Broadcast(CurrentHP);

	// 플레이어만 무적 시간 적용
	if (bIsPlayer && !bInvincible)
	{
		bInvincible = true;
		GetWorld()->GetTimerManager().SetTimer(
			InvincibleTimerHandle, this, &UDEStatComponent::EndInvincible, InvincibleTime, false
		);
	}

	if (CurrentHP <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Took Damage"));
		CurrentHP = 0.f;
		OnZeroHP.Broadcast(); // Monster/Player에서 바인딩
	}

	return DamageAmount;
}

void UDEStatComponent::Heal(float Amount)
{
	if (Amount <= 0.f) return;

	// HP 증가, MaxHP를 넘어가지 않도록 클램프
	CurrentHP = FMath::Clamp(CurrentHP + Amount, 0.f, MaxHP);

	// HP 변경 이벤트 브로드캐스트
	OnHPChanged.Broadcast(CurrentHP);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.0f,
			FColor::Green,
			FString::Printf(TEXT("%s Healed: %.1f, CurrentHP: %.1f"), *GetOwner()->GetName(), Amount, CurrentHP)
		);
	}

}

void UDEStatComponent::AddEXP(float v)
{
	CurrentEXP += v;
	while (CurrentEXP >= NextLevelEXP)
	{
		CurrentEXP -= NextLevelEXP;  // 남은 경험치
		LevelUp();                      // 레벨업 처리
	}
}

void UDEStatComponent::LevelUp()
{
	Level++;
	CurrentEXP = 0.f;
	NextLevelEXP *= 1.3f;  // 필요하면 조절
	UE_LOG(LogTemp, Warning, TEXT("LEVEL UP! New Level: %d"), Level);

	OnLevelUp.Broadcast();
}
