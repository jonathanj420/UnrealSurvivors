// Fill out your copyright notice in the Description page of Project Settings.


#include "DEResultWidget.h"
#include "Kismet/GameplayStatics.h"

void UDEResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼에 클릭 함수 연결
	if (Btn_Retry)
		Btn_Retry->OnClicked.AddDynamic(this, &UDEResultWidget::OnRetryClicked);

	if (Btn_MainMenu)
		Btn_MainMenu->OnClicked.AddDynamic(this, &UDEResultWidget::OnMainMenuClicked);
}

void UDEResultWidget::SetResultData(int32 EarnedGold, int32 KillCount, float DamageDealt, float SurviveTime)
{
	// 1. 화면에 텍스트 갱신
	if (Text_GoldEarned)
		Text_GoldEarned->SetText(FText::AsNumber(EarnedGold));

	if (Text_KillCount)
		Text_KillCount->SetText(FText::AsNumber(KillCount));

	if (Text_DamageDealt)
		Text_DamageDealt->SetText(FText::AsNumber(FMath::RoundToInt(DamageDealt)));

	// 시간 포맷 (분:초) 변환
	FTimespan Time(0, 0, (int32)SurviveTime);
	if (Text_SurviveTime)
		Text_SurviveTime->SetText(FText::AsTimespan(Time));

	// 골드 정산/저장은 ADEGameMode_Stage::GameOver에서 처리 (위젯은 표시만)
}

void UDEResultWidget::OnRetryClicked()
{
	// 현재 레벨 다시 시작
	UGameplayStatics::SetGamePaused(this, false);

	FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevel));
}

void UDEResultWidget::OnMainMenuClicked()
{
	// 메인 메뉴 레벨로 이동 (레벨 이름 확인 필요!)
	UGameplayStatics::OpenLevel(this, FName("L_Lobby"));
}
