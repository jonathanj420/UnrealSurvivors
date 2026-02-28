// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBase.h"
#include "Data/DESkillData.h"
#include "DESkillContext.h"
#include "DEAutoSkillBase.generated.h"


/**
 * 
 */
UCLASS(Abstract)
class DARKEDENSURVIVORS_API UDEAutoSkillBase : public UDESkillBase
{
	GENERATED_BODY()

public:
	// 외부(매니저)에서 호출하는 실행 함수
	virtual void Activate();

protected:
	// ★ 새로 만든 가상 함수! 자식들이 오버라이드하기 딱 좋게 Context를 던져줍니다.
	virtual void ExecuteWithContext(FDESkillContext& Context);

public:	// 데이터 주입 (레벨업 시 호출)
	void SetSkillData(const FDESkillData* NewData);

	// 자식 클래스에서 Behavior 조립
	virtual void InitBehaviors();

protected:
	virtual void BuildContext(FDESkillContext& OutContext);

protected:
	UPROPERTY()
	TArray<class UDESkillBehavior*> Behaviors;

	// 현재 스킬 스펙 (포인터만 참조)
	const FDESkillData* SkillData = nullptr;
};
