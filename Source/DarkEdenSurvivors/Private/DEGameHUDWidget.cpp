// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameHUDWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "DECharacterBase.h"
#include "DEHealthComponent.h"
#include "DEProgressionComponent.h"
#include "DESkillInventoryWidget.h"
#include "DEAccInventoryWidget.h"


void UDEGameHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UDEGameHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // 월드 시간을 가져와서 바로 업데이트
    if (GetWorld())
    {
        float TimeSeconds = GetWorld()->GetTimeSeconds();

        // [최적화] 1초에 한 번만 텍스트 갱신 (매 프레임 문자열 연산 방지)
        int32 CurrentSeconds = (int32)TimeSeconds;
        if (CurrentSeconds != LastSeconds)
        {
            LastSeconds = CurrentSeconds;
            UpdateTime(TimeSeconds);
        }
    }
}

void UDEGameHUDWidget::BindCharacterStat(ADECharacterBase* Character)
{
    if (!Character) return;

    // 1. 체력 컴포넌트 연결
    if (UDEHealthComponent* HealthComp = Character->GetHealthComponent())
    {
        // 초기값 한 번 갱신
        UpdateHP(HealthComp->GetCurrentHP(), HealthComp->GetMaxHP());

        // 이벤트 구독 (AddUObject로 묶어야 안전)
        // 주의: HealthComponent의 델리게이트 시그니처가 (Current, Max)를 보내줘야 함
        HealthComp->OnHPChanged.AddUObject(this, &UDEGameHUDWidget::UpdateHP);
    }

    // 2. 경험치 컴포넌트 연결 (구현되어 있다면)

    if (UDEProgressionComponent* ProgressionComponent = Character->GetProgressionComponent())
    {
        UpdateExp(ProgressionComponent->GetCurrentExp(), ProgressionComponent->GetMaxExp());
        ProgressionComponent->OnExpChanged.AddUObject(this, &UDEGameHUDWidget::UpdateExp);
    }

    // 2. 스킬 위젯도 연결해라! (여기가 핵심)
    if (WBP_SkillInventoryWidget)
    {
        WBP_SkillInventoryWidget->InitializeSkillWidget(Character);
    }

    if (WBP_AccInventoryWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("Try Acc Inv"));
        WBP_AccInventoryWidget->InitializeAccessoryWidget(Character);
    }
    UE_LOG(LogTemp, Warning, TEXT("Stat Bound successfully"));

}

void UDEGameHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
    if (PB_HP && MaxHP > 0.0f)
    {
        // 0.0 ~ 1.0 사이 비율로 변환
        float Percent = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);
        PB_HP->SetPercent(Percent);
    }
}

void UDEGameHUDWidget::UpdateExp(float CurrentExp, float MaxExp)
{
    if (PB_Exp && MaxExp > 0.0f)
    {
        float Percent = FMath::Clamp(CurrentExp / MaxExp, 0.0f, 1.0f);
        PB_Exp->SetPercent(Percent);
    }
}

void UDEGameHUDWidget::UpdateTime(float TimeSeconds)
{
    int32 Minutes = TimeSeconds / 60;
    int32 Seconds = (int32)TimeSeconds % 60;

    FString TimeStr = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

    if (Text_Time)
        Text_Time->SetText(FText::FromString(TimeStr));
}