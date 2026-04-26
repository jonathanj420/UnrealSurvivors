// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DESkillBehavior.generated.h"

/**
 * 
 */
struct FDESkillContext;

/**
 * 모든 스킬 행동의 베이스 클래스
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
class DARKEDENSURVIVORS_API UDESkillBehavior : public UObject
{
    GENERATED_BODY()

public:
    /** 실제 행동 실행 */
    virtual void Execute(FDESkillContext& Context) PURE_VIRTUAL(UDESkillBehavior::Execute, );
    virtual void OnContextRefreshed(const FDESkillContext& Context) {}
    // 새롭게 추가! 스킬이 끝날 때 비헤이비어 자신이 정리할 게 있다면 여기서 하도록 뚫어줌
    virtual void EndBehavior() {}
public:
    // 파이프라인 루프를 멈춰야 하는 시간이 있는지 반환합니다.
    // 기본값은 0.0f 이며, 딜레이가 없는 일반 비헤이비어들은 오버라이드 할 필요가 없습니다.
    virtual float GetPipelineDelay() const { return 0.0f; }

};