// Fill out your copyright notice in the Description page of Project Settings.


#include "DEPauseMenuWidget.h"
#include "Components/Button.h"
#include "DEPlayerController.h"
#include "DEGameMode_Stage.h"
#include "Kismet/GameplayStatics.h"

void UDEPauseMenuWidget::NativeConstruct()
{
    Super::NativeConstruct();


}

void UDEPauseMenuWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    // 1. 계속하기 버튼 연결
    if (Btn_Resume)
    {
        Btn_Resume->OnClicked.AddDynamic(this, &UDEPauseMenuWidget::OnResumeClicked);
    }

    // 2. 나가기 버튼 연결
    if (Btn_Exit)
    {
        Btn_Exit->OnClicked.AddDynamic(this, &UDEPauseMenuWidget::OnExitClicked);
    }

}


void UDEPauseMenuWidget::OnResumeClicked()
{
    // 컨트롤러를 찾아서 다시 Toggle (끄기) 실행
    if (ADEPlayerController* PC = Cast<ADEPlayerController>(GetOwningPlayer()))
    {
        PC->TogglePauseMenu();
    }

}

void UDEPauseMenuWidget::OnExitClicked()
{
    // 게임오버를 먼저 걸어야 메뉴를 닫는 순간 일시정지가 풀리지 않는다
    ADEGameMode_Stage* GameMode = Cast<ADEGameMode_Stage>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GameMode)
    {
        // 알아서 시간 멈추고 결산 위젯 띄워줄 거임
        GameMode->GameOver();
    }

    // 결산 UI와 겹치지 않게 메뉴를 닫는다
    if (ADEPlayerController* PC = Cast<ADEPlayerController>(GetOwningPlayer()))
    {
        PC->ClosePauseMenu();
    }
    else
    {
        RemoveFromParent();
    }
}
