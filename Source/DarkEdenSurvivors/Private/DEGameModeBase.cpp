// Fill out your copyright notice in the Description page of Project Settings.


#include "DEGameModeBase.h"
#include "DEGameInstance.h" // 인스턴스 헤더 필수
#include "Kismet/GameplayStatics.h"


ADEGameModeBase::ADEGameModeBase()
{
	// 기본 폰이나 컨트롤러는 자식에서 설정하므로 여기선 비워둬도 됨
	// 필요하다면 가장 기본이 되는 컨트롤러 설정
	// PlayerControllerClass = AMyBaseController::StaticClass();
}

void ADEGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// 접속 로그 (로비든 스테이지든 다 뜸)
	UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] Player Login: %s"), *NewPlayer->GetName());

	// 여기서 GameInstance의 데이터를 가져와서 플레이어에게 적용할 수도 있음
	// 예: NewPlayer->SetSkin(GameInstance->GetEquippedSkin());
}

void ADEGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	UE_LOG(LogTemp, Warning, TEXT("[GameModeBase] Player Logout: %s"), *Exiting->GetName());

	// (선택) 맵 나갈 때 자동 저장?
	// SaveGameData();
}

void ADEGameModeBase::SaveGameData()
{
	// 매번 Cast하기 귀찮으니까 여기서 한 번에 처리
	if (UDEGameInstance* GI = Cast<UDEGameInstance>(GetGameInstance()))
	{
		GI->SaveGame();
	}
}

void ADEGameModeBase::Cheat_AddGold(int32 Amount)
{
	if (UDEGameInstance* GI = Cast<UDEGameInstance>(GetGameInstance()))
	{
		GI->AddGold(Amount);
		GI->SaveGame();
		UE_LOG(LogTemp, Warning, TEXT("[Cheat] Add Gold: %d"), Amount);
	}
}