// Fill out your copyright notice in the Description page of Project Settings.


#include "DEResultWidget.h"
#include "Kismet/GameplayStatics.h"
#include "DEGameInstance.h" // ★ 저장 기능 사용을 위해 필수

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

	// 2. ★ [핵심] 획득한 골드를 영구 저장소(GameInstance)에 저장
	if (UDEGameInstance* GI = Cast<UDEGameInstance>(GetGameInstance()))
	{
		//UE_LOG(LogTemp, Warning, TEXT("[Result] Saving Gold: +%d"), EarnedGold);

		//GI->AddGold(EarnedGold); // 메모리에 더하기
		GI->SaveGame();          // 파일로 저장하기 (SaveSlot01.sav)
	}
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
