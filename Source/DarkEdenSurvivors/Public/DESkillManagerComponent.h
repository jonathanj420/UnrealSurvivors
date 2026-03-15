// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DESkillData.h"
#include "Data/DEEvolutionData.h"
#include "DESkillTypes.h"
#include "DESkillManagerComponent.generated.h"



DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillUpdated, int32 /*SkillID*/);

class UDEAutoSkillBase;
class UDEInventoryComponent;

USTRUCT()
struct FActiveSkill
{
	GENERATED_BODY()

	UPROPERTY()
	UDEAutoSkillBase* SkillObject = nullptr;

	const FDESkillData* RowData = nullptr;   // 데이터 테이블 Row
	UPROPERTY()
	int32 SkillID = 0; // SkillID만 가지고 있으면 됨
	UPROPERTY()
	int32 CurrentLevel = 1;

	float CurrentCooldown = 0.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDESkillManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDESkillManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ApplyCharacterDamageMultiplier(float Multiplier); // 캐릭터 공격력 보정

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	UPROPERTY(EditAnywhere, Category = "SkillTable")
	UDataTable* SkillRowTable;

	UPROPERTY(EditAnywhere)
	UDataTable* SkillDataTable;

	void LoadSkillRowTable();
	void LoadSkillDataTable();
	//current skills
	TMap<int32, int32> SkillLevels;          // SkillID -> Level

	UPROPERTY(VisibleAnywhere, Category = "Skills")
	TMap<int32, FActiveSkill> ActiveSkills; // SkillID -> ActiveSkill
	TMap<int32, TMap<int32, FDESkillData>> InitializedSkills;
	TMap<int32, FDESkillRow> SkillRowMap;

	void InitSkills();
	//FSkillSpec MakeSkillSpec(int32 SkillID);
	float DeltaCheck = 0.0f;

public:
	UFUNCTION()
	void LevelUpSkill(int32 SkillID);

	// 스킬 레벨 가져오기
	int32 GetSkillLevel(int32 SkillID) const
	{
		// TMap에서 구조체의 포인터를 찾습니다. (없으면 nullptr 반환)
		const FActiveSkill* FoundSkill = ActiveSkills.Find(SkillID);

		// 스킬이 존재하면 그 안의 CurrentLevel을, 없으면 0을 리턴합니다.
		return FoundSkill ? FoundSkill->CurrentLevel : 0;
	}

	// 스킬 보유 여부
	bool HasSkill(int32 SkillID) const
	{
		return ActiveSkills.Contains(SkillID);

		//return SkillLevels.Contains(SkillID);
	}
	// DataTable에 특정 레벨의 row 존재하는지
	bool HasSkillData(int32 SkillID, int32 Level) const;
	// 보유 스킬 리스트
	TArray<int32> GetOwnedSkillIDs() const;

	// 후보 스킬 목록 (현재 스킬의 다음 레벨만)
	TArray<int32> GetUpgradeableSkills() const;

	FDESkillRow* GetSkillRow(int32 SkillID);

public:
	// 스킬 목록을 외부에서 '읽기 전용'으로 볼 수 있게 해주는 Getter
	const TMap<int32, FActiveSkill>& GetActiveSkills() const { return ActiveSkills; }
	const TMap<int32, int32>& GetSkillLevels() const { return SkillLevels; }
public:

	TArray<FDESkillData*> GetRandomSkillChoices(int32 Count = 3);
	const FDESkillRow* GetRandomSkillRow();
	UFUNCTION(BlueprintCallable)
	void ApplySkillChoice(int32 SkillID);
protected:
	UDataTable* EvolutionDataTable;
	void LoadEvolutionDataTable();
public:

	bool CheckEvolution(int32& OutBaseSkillID, int32& OutResultSkillID);
	void EvolveSkill(int32 BaseSkillID, int32 ResultSkillID);


protected:
	bool bAutoSkillPaused = false;

public:
	void PauseAutoSkills();
	void ResumeAutoSkills();
	bool IsAutoSkillPaused() const { return bAutoSkillPaused; }
	FOnSkillUpdated OnSkillUpdated;

protected:
	UPROPERTY()
	class UDEStatComponent* CachedStatComp;
	UPROPERTY()
	class UDEInventoryComponent* CachedInventoryComp;

public:
	void InitStatComp(class UDEStatComponent* InStatComp) { CachedStatComp = InStatComp; }
	void InitInventoryComp(class UDEInventoryComponent* InInventoryComp) { CachedInventoryComp = InInventoryComp; }


public:
	// 1. 특정 스킬 쿨타임 즉시 초기화 (부메랑 회수, 특정 조건 달성 시)
	UFUNCTION(BlueprintCallable, Category = "Skill|Cooldown")
	void ResetCooldownInstant(int32 SkillID);

	// 2. 특정 스킬 쿨타임 N초 감소 (평타 타격 시 특정 스킬 쿨감 효과 등)
	UFUNCTION(BlueprintCallable, Category = "Skill|Cooldown")
	void ReduceCooldown(int32 SkillID, float Amount, ECooldownReduceType ReduceType = ECooldownReduceType::Flat);

	// 3. 모든 스킬 쿨타임 N초 감소 (광역 쿨감 아이템 획득, 특수 버프 등)
	UFUNCTION(BlueprintCallable, Category = "Skill|Cooldown")
	void ReduceAllCooldowns(float ReduceAmount);
protected:
	float TotalReducedCooldownAmount = 0.0f;



};
