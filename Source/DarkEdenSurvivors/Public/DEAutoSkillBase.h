// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBase.h"
#include "Data/DESkillData.h"
#include "DESkillContext.h"
#include "DECombatEffect.h"
#include "DEDamageInstigatorInterface.h"
#include "DEAutoSkillBase.generated.h"


/**
 * 
 */
UCLASS(Abstract)
class DARKEDENSURVIVORS_API UDEAutoSkillBase : public UDESkillBase, public IDEDamageInstigatorInterface
{
	GENERATED_BODY()

public:
	// 외부(매니저)에서 호출하는 실행 함수
	virtual void Activate();
	// ★ 매니저가 이 스킬이 '시전(유지) 중'인지 확인하는 용도
	bool IsRunning() const { return bIsRunning; }

protected:
	// ★ 이 스킬이 '지속시간이 끝난 후 쿨타임이 도는' 성서 타입인가?
	// (나중에 성서(DanseMacabre) 자식 클래스 C++ 생성자나 블루프린트에서 이걸 true로 체크하면 됨!)
	UPROPERTY(EditDefaultsOnly, Category = "Skill Option")
	bool bCooldownAfterDuration = false;

	bool bIsRunning = false;
	FTimerHandle DurationTimerHandle;

	// 지속시간이 끝났을 때 호출될 함수
	virtual void FinishSkill();

protected:
	FDESkillContext CachedContext;
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

public:
	UPROPERTY(EditAnywhere, Instanced, Category = "Combat Effects")
	TArray<UDECombatEffect*> LocalEffects;

public:
	TMap<FName, TWeakObjectPtr<class ADESimpleAOEBase>> OwnedAOEMap;
public:
	virtual void EndSkill();
public:
	// ★ 인터페이스 함수 선언 (기본적으로는 아무것도 안 함)
	virtual void OnTargetKilled(const FDEDamageResult& Result) override;
	int32 GetBaseAmount() const;

};
