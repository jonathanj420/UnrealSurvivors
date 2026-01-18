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
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class DARKEDENSURVIVORS_API UDESkillBehavior : public UObject
{
    GENERATED_BODY()

public:
    /** 실제 행동 실행 */
    virtual void Execute(FDESkillContext& Context) PURE_VIRTUAL(UDESkillBehavior::Execute, );
};