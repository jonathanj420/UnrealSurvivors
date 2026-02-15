#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DEMonsterData.generated.h"

/**
 * 몬스터의 모든 정보(스탯, 외형, 보상 등)를 정의하는 데이터 테이블용 구조체
 */
USTRUCT(BlueprintType)
struct FDEMonsterData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// [1. 외형 데이터 (핵심)]
	// 메모리 관리를 위해 Soft Reference 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<USkeletalMesh> MonsterMesh;

	// AnimBP도 필요 없음. 그냥 '걷기 모션' 파일 하나만 있으면 됨.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<UAnimSequence> WalkAnim;

	// 메쉬마다 중심점이 다르니 높이 보정용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float MeshZOffset = 0.0f; // 예: -90.0f

	// 메쉬마다 덩치가 다르니 캡슐 크기 보정용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float CapsuleRadius = 42.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float CapsuleHalfHeight = 96.0f;

	// 크기 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float ScaleMultiplier = 1.0f;

	// [2. 스탯 데이터 (기존 동일)]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHP = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MoveSpeed = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackDamage = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackInterval = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float KnockbackResistance = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float EXPDrop = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float DropChance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System")
	TSoftClassPtr<class ADEMonsterBase> OverrideClass;

};