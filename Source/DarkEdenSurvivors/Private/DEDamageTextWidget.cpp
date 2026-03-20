// Fill out your copyright notice in the Description page of Project Settings.


#include "DEDamageTextWidget.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"

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
	bActive = true;
	SetVisibility(ESlateVisibility::HitTestInvisible);

	//  Enum을 이용한 분기 처리 (확장성 MAX)
	switch (Info.TextType)
	{
	case EDamageTextType::Execution:
	{
		DamageText->SetText(FText::FromString(TEXT("EXECUTED!")));
		//DamageText->SetColorAndOpacity(FSlateColor(FLinearColor(0.6f, 0.0f, 0.8f, 1.0f))); // 다크 퍼플 (사신 느낌)
		DamageText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		// 처형은 크기를 1.5배 뻥튀기!
		ScaleContainer->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.5f, 1.5f), FVector2D::ZeroVector, 0.0f));

		break;
	}
	case EDamageTextType::Critical:
	{
		DamageText->SetText(FText::AsNumber(FMath::RoundToInt(Info.Amount)));
		DamageText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		ScaleContainer->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.2f, 1.2f), FVector2D::ZeroVector, 0.0f));
		break;
	}
	case EDamageTextType::Heal:
	{
		DamageText->SetText(FText::AsNumber(FMath::RoundToInt(Info.Amount)));
		DamageText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
//		DamageText->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.0f, 1.0f), FVector2D::ZeroVector, 0.0f));
		DamageText->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.0f, 1.0f), FVector2D::ZeroVector, 0.0f));
		break;
	}
	default: // Damage (일반 타격)
	{
		DamageText->SetText(FText::AsNumber(FMath::RoundToInt(Info.Amount)));
		DamageText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		DamageText->SetRenderTransform(FWidgetTransform(FVector2D::ZeroVector, FVector2D(1.0f, 1.0f), FVector2D::ZeroVector, 0.0f));
		break;
	}
	}

	// 애니메이션 재생
	if (PopAnimMk2)
	{
		PlayAnimation(PopAnimMk2);
	}
    //TargetWorldLocation = Info.WorldLocation;

    //DamageText->SetText(FText::AsNumber(FMath::RoundToInt(Info.Amount)));
    //DamageText->SetColorAndOpacity(
    //    Info.bIsCritical ? FLinearColor::Red : FLinearColor::White
    //);

    //bActive = true;
    ////SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
    //SetVisibility(ESlateVisibility::HitTestInvisible);

    //if (PopAnimMk2)
    //{
    //    PlayAnimation(PopAnimMk2);


    //}
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