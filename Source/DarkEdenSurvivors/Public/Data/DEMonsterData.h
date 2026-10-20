#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "DEPickupBase.h"
#include "DEMonsterData.generated.h"

UENUM(BlueprintType)
enum class EDEMonsterRank : uint8
{
	Normal  UMETA(DisplayName = "Normal Mob"),
	Elite   UMETA(DisplayName = "Elite Mob"),
	Boss    UMETA(DisplayName = "Boss Mob")
};

// 스폰 시점에 DT 값에 곱하는 배율 (난이도 곡선 × Curse). ADEGameMode_Stage::GetMonsterStatScale
struct FDEMonsterStatScale
{
	float HP = 1.0f;
	float Damage = 1.0f;
	float MoveSpeed = 1.0f;
};

USTRUCT(BlueprintType)
struct FDEMonsterDropInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	TSubclassOf<ADEPickupBase> DropClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	float DropChance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	float ItemValue = 10.0f;
};

/**
 * 몬스터의 모든 정보(스탯, 외형, 보상 등)를 정의하는 데이터 테이블용 구조체
 */
USTRUCT(BlueprintType)
struct FDEMonsterData : public FTableRowBase
{
	GENERATED_BODY()

public:
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop")
	TArray<FDEMonsterDropInfo> DropTable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System")
	EDEMonsterRank MonsterRank = EDEMonsterRank::Normal;
	// 광흡(BloodDrain) 면역. 보스는 이 값과 상관없이 항상 면역
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System")
	bool bImmuneToBloodDrain = false;
	// 상태이상 면역. 효과의 StatusTags와 하나라도 겹치면 걸리지 않는다 (예: Status.Debuff.Stun)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System", meta = (Categories = "Status"))
	FGameplayTagContainer ImmuneStatusTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "System")
	TSoftClassPtr<class ADEMonsterBase> OverrideClass;

	// [1. 외형 데이터 (핵심)]
	// 메모리 관리를 위해 Soft Reference 사용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<USkeletalMesh> MonsterMesh;

	// AnimBP도 필요 없음. 그냥 '걷기 모션' 파일 하나만 있으면 됨.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<UAnimSequence> WalkAnim;

	// AnimToTexture로 구운 VAT 데이터. 지정하면 MonsterMesh 대신 이 에셋의 StaticMesh를 ISM 인스턴스로 그린다 (UDEMonsterRenderComponent).
	// 비어 있거나 보스면 MonsterMesh(SkeletalMesh)로 그린다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	TSoftObjectPtr<class UAnimToTextureDataAsset> VATData;

	// VATData의 Animations 중 반복 재생할 것 (걷기)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	int32 VATAnimationIndex = 0;

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


};