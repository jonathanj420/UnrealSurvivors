// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_PlayUI.h"
#include "DESkillContext.h" 
#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

void UDEBehavior_PlayUI::Execute(FDESkillContext& Context)
{
    if (CinematicWidgetClass && Context.Instigator)
    {
        if (APawn* InstigatorPawn = Cast<APawn>(Context.Instigator))
        {
            // 2. 이제 Pawn에서 Controller를 가져와 다시 PlayerController로 캐스팅합니다.
            if (APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController()))
            {
                UUserWidget* CinematicUI = CreateWidget<UUserWidget>(PC, CinematicWidgetClass);
                if (CinematicUI)
                {
                    // 화면 최상단에 UI를 띄웁니다!
                    CinematicUI->AddToViewport(ZOrder);
                }
            }
        }

    }
}
