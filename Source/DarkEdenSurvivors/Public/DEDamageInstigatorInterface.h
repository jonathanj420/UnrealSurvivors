// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DEDamageTypes.h"
#include "DEDamageInstigatorInterface.generated.h"

// 함수가 C++ 가상 함수뿐이라 BP에서는 구현할 수 없다
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UDEDamageInstigatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DARKEDENSURVIVORS_API IDEDamageInstigatorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 데미지 유발자(스킬 등)가 막타를 쳤을 때 호출될 가상 함수
	virtual void OnTargetKilled(const FDEDamageResult& Result) = 0;
};
