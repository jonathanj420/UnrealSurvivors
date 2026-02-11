// Fill out your copyright notice in the Description page of Project Settings.


#include "DEDamageTextWidget.h"
#include "Components/TextBlock.h"

void UDEDamageTextWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetVisibility(ESlateVisibility::Collapsed);
    //SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
    //SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
    // [이동] 여기서 딱 한 번만 연결하세요!
    if (PopAnimMk2)
    {
        // [수정된 부분]
        // 1. 델리게이트 변수를 하나 만듭니다.
        FWidgetAnimationDynamicEvent AnimFinishedDelegate;

        // 2. 'BindDynamic'을 써서 함수를 연결합니다. (CreateUObject 아님!)
        AnimFinishedDelegate.BindDynamic(this, &UDEDamageTextWidget::HandleAnimFinished);

        // 3. 연결된 델리게이트를 등록합니다.
        BindToAnimationFinished(PopAnimMk2, AnimFinishedDelegate);
    }
}

void UDEDamageTextWidget::Activate(const FDamageVisualInfo& Info)
{
    TargetWorldLocation = Info.WorldLocation;

    DamageText->SetText(FText::AsNumber(FMath::RoundToInt(Info.Amount)));
    DamageText->SetColorAndOpacity(
        Info.bIsCritical ? FLinearColor::Red : FLinearColor::White
    );

    bActive = true;
    //SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
    SetVisibility(ESlateVisibility::HitTestInvisible);

    if (PopAnimMk2)
    {
        PlayAnimation(PopAnimMk2);


    }
}

void UDEDamageTextWidget::HandleAnimFinished()
{
    Deactivate();
}

void UDEDamageTextWidget::Deactivate()
{
    bActive = false;
    SetVisibility(ESlateVisibility::Collapsed);
}