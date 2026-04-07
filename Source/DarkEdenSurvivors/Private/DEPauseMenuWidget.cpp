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
    // 1. 현재 화면을 가리고 있는 일시정지 메뉴(나 자신)를 먼저 화면에서 치운다!
    // (이거 안 하면 나중에 띄울 결산 UI랑 화면에 겹쳐서 난장판 됨)
    this->RemoveFromParent();

    // 2. 심판(GameMode)을 불러온다!
    ADEGameMode_Stage* GameMode = Cast<ADEGameMode_Stage>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GameMode)
    {
        // 3. 심판에게 항복 선언! (알아서 시간 멈추고 결산 위젯 띄워줄 거임)
        GameMode->GameOver();
    }
}
