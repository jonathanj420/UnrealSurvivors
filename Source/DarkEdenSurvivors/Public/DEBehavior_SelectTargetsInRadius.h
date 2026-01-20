// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillBehavior.h"
#include "DEBehavior_SelectTargetsInRadius.generated.h"

UENUM(BlueprintType)
enum class ESearchOrigin : uint8
{
    //Instigator,      // 시전자(플레이어) 기준 -> 베놈 서클
    //CurrentTarget,   // 현재 타겟 기준 -> 블러드 스피어 (스플래시)
    //CustomLocation   // 특정 좌표 기준 -> 애시드 이럽션 (낙뢰)

    Instigator,      // 시전자 (플레이어 1명)
    Targets,         // 변경: 현재 타겟 리스트 '전체' (각각의 위치가 모두 중심점이 됨)
    CustomLocations  // 변경: 저장된 좌표 리스트 '전체' (성수 여러 발)
};

/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEBehavior_SelectTargetsInRadius : public UDESkillBehavior
{
	GENERATED_BODY()

public:
    virtual void Execute(FDESkillContext& Context) override;

    // 1. 검색 반경 (이건 고정값이라 숫자 입력)
    UPROPERTY(EditAnywhere, Category = "Search")
    float Radius = 400.f;

    // 2. 기준점 타입 (이건 '방법'이라 Enum 입력)
    UPROPERTY(EditAnywhere, Category = "Search")
    ESearchOrigin OriginType = ESearchOrigin::Instigator;

    // 3. (Custom일 때) Context에서 읽어올 좌표 이름
    UPROPERTY(EditAnywhere, Category = "Search", meta = (EditCondition = "OriginType == ESearchOrigin::CustomLocation"))
    FName VectorKeyName = TEXT("TargetLocation");

    // 4. 시전자는 결과에서 뺄까?
    UPROPERTY(EditAnywhere, Category = "Search")
    bool bIgnoreInstigator = true;

    // ★ 추가: 중복 타격 허용 여부
    // true: 장판 3개 겹치면 3번 리스트에 넣음 (3배 데미지)
    // false: 장판 100개 겹쳐도 1번만 넣음 (1배 데미지)
    UPROPERTY(EditAnywhere, Category = "Search")
    bool bAllowOverlap = false;
	
};
