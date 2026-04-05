#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "DEStatTypes.h"
#include "DESkillContext.generated.h"

class AActor;
struct FDESkillData;
class UDEAutoSkillBase;


/**
 * 스킬 실행 시 전달되는 실행 컨텍스트
 */
USTRUCT()
struct FDESkillContext
{
	GENERATED_BODY()

public:
	// --- [1. 필수 참조 데이터] ---
	// GC가 건드리지 못하게 UPROPERTY() 필수
	UPROPERTY(Transient)
	AActor* Instigator = nullptr;

	UPROPERTY()
	FCombatSnapshot FinalSnapshot;

	UPROPERTY(Transient)
	TArray<AActor*> Targets;


	// Spawn 결과 기록 (선택, SpawnAOE에서 사용)
	TArray<TWeakObjectPtr<class ADESimpleAOEBase>> SpawnedAOEs;

	// [중요 추가] 이 컨텍스트를 만든 스킬 인스턴스 (상태 접근용)
	// 예: SourceSkill->SpawnedAura 에 접근하기 위해 필수
	UPROPERTY(Transient)
	UDEAutoSkillBase* SourceSkill = nullptr;
	// --- [2. 메이저 스탯 (Raw C++ Type)] ---
	// 일반 변수는 UPROPERTY 없어도 되지만, TMap 직렬화/초기화를 위해 남김
	// (필요 없으면 float는 그냥 float로 써도 됩니다. 여기선 통일성을 위해 둠)

	float Damage = 0.f;
	int32 Amount = 1;
	int32 Penetration = 0;
	float Speed = 1000.f;
	float Radius = 100.0f;
	float KnockbackForce = 600.0f;
	float Duration = 0.f;
	float CritChance = 0.f;
	float CritDamageMultiplier = 2.0f;
	// --- [3. 확장 데이터] ---
	UPROPERTY(Transient)
	TMap<FName, float> CustomValues;

	// 헬퍼 함수
	float GetValue(FName Key, float DefaultValue = 0.f) const
	{
		if (const float* Val = CustomValues.Find(Key))
		{
			return *Val;
		}
		return DefaultValue;
	}

	UPROPERTY(Transient)
	TArray<FVector> CustomLocations; // 다중 좌표 저장용
};

