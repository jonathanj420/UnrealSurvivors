// Fill out your copyright notice in the Description page of Project Settings.


#include "DEChestRewardSlotWidget.h"
#include "DELevelUpChoiceBase.h" // 네가 만든 Choice 기본 클래스
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UDEChestRewardSlotWidget::InitSlot(UDELevelUpChoiceBase* RewardData)
{
	if (!RewardData) return;

	// 1. 아이콘 세팅 (네가 만든 GetIcon() 함수 사용!)
	if (Img_RewardIcon && RewardData->GetIcon())
	{
		Img_RewardIcon->SetBrushFromTexture(RewardData->GetIcon());
	}

	// 2. 이름 세팅 (네가 만든 GetDisplayName() 함수 사용!)
	if (Txt_RewardName)
	{
		Txt_RewardName->SetText(RewardData->GetDisplayName());
	}

	// 3. (선택) 진화 스킬 텍스트 처리 
	//if (Txt_LevelInfo)
	//{
	//	// 나중에 필요하면 ChoiceBase에 IsEvolution() 같은 가상 함수 뚫어서 판별하면 됨!
	//	Txt_LevelInfo->SetText(FText::FromString(TEXT("Level Up!")));
	//}

	// 4. 데이터 세팅 끝! 블루프린트 연출(빛무리 회전 등) 큐!
	OnSlotRevealed();
}
