// Fill out your copyright notice in the Description page of Project Settings.


#include "DESaveGame.h"

UDESaveGame::UDESaveGame()
{
	SaveSlotName = TEXT("DESaveSlot01");
	UserIndex = 0;
	TotalGold = 0; // 초기 자금 0원
}