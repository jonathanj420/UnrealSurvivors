// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DESkillData.h"
#include "DESkillManagerComponent.generated.h"

class UDESkillBase;


USTRUCT()
struct FActiveSkill
{
	GENERATED_BODY()

	UPROPERTY()
	UDESkillBase* SkillObject = nullptr;

	FDESkillData* RowData = nullptr;   // 데이터 테이블 Row
	UPROPERTY()
	int32 SkillID = 0; // SkillID만 가지고 있으면 됨

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
	void AddSkill(int32 SkillID);
	UFUNCTION()
	void LevelUpSkill(int32 SkillID);
		
	// 스킬 레벨 가져오기
	int32 GetSkillLevel(int32 SkillID) const
	{
		const int32* Lv = SkillLevels.Find(SkillID);
		return Lv ? *Lv : 0;
	}

	// 스킬 보유 여부
	bool HasSkill(int32 SkillID) const
	{
		return SkillLevels.Contains(SkillID);
	}
	// DataTable에 특정 레벨의 row 존재하는지
	bool HasSkillData(int32 SkillID, int32 Level) const;
	// 보유 스킬 리스트
	TArray<int32> GetOwnedSkillIDs() const;

	// 후보 스킬 목록 (현재 스킬의 다음 레벨만)
	TArray<int32> GetUpgradeableSkills() const;

public:

	TArray<FDESkillData*> GetRandomSkillChoices(int32 Count = 3);

	UFUNCTION(BlueprintCallable)
	void ApplySkillChoice(int32 SkillID);

	


};
