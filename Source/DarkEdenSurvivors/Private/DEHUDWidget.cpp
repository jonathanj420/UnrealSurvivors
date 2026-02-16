// Fill out your copyright notice in the Description page of Project Settings.


#include "DEHUDWidget.h"
#include "Components/ProgressBar.h"
#include "DECharacterBase.h"
#include "DEHealthComponent.h"
#include "DEProgressionComponent.h"

void UDEHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UDEHUDWidget::BindCharacterStat(ADECharacterBase* Character)
{
    if (!Character) return;

    // 1. 체력 컴포넌트 연결
    if (UDEHealthComponent* HealthComp = Character->GetHealthComponent())
    {
        // 초기값 한 번 갱신
        UpdateHP(HealthComp->GetCurrentHP(), HealthComp->GetMaxHP());

        // 이벤트 구독 (AddUObject로 묶어야 안전)
        // 주의: HealthComponent의 델리게이트 시그니처가 (Current, Max)를 보내줘야 함
        HealthComp->OnHPChanged.AddUObject(this, &UDEHUDWidget::UpdateHP);
    }

    // 2. 경험치 컴포넌트 연결 (구현되어 있다면)
    
    if (UDEProgressionComponent* ProgressionComponent = Character->GetProgressionComponent())
    {
        UpdateExp(ProgressionComponent->GetCurrentExp(), ProgressionComponent->GetMaxExp());
        ProgressionComponent->OnExpChanged.AddUObject(this, &UDEHUDWidget::UpdateExp);
    }
    
}

void UDEHUDWidget::UpdateHP(float CurrentHP, float MaxHP)
{
    if (PB_HP && MaxHP > 0.0f)
    {
        // 0.0 ~ 1.0 사이 비율로 변환
        float Percent = FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);
        PB_HP->SetPercent(Percent);
    }
}

void UDEHUDWidget::UpdateExp(float CurrentExp, float MaxExp)
{
    if (PB_Exp && MaxExp > 0.0f)
    {
        float Percent = FMath::Clamp(CurrentExp / MaxExp, 0.0f, 1.0f);
        PB_Exp->SetPercent(Percent);
    }
}
