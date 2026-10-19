// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DEInventoryComponent.generated.h"

class UDEAutoSkillBase;

DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged);

UENUM(BlueprintType)
enum class EInventorySlotType : uint8
{
	Skill,
	Accessory
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKEDENSURVIVORS_API UDEInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDEInventoryComponent();

protected:
    virtual void BeginPlay() override;

public:
    // 스킬 보유와 슬롯은 UDESkillManagerComponent가 관리한다

    /* ================= 슬롯 제한 ================= */

    UPROPERTY(EditAnywhere, Category = "Inventory|Slots")
    int32 MaxAccessorySlots = 6;

    /* ================= 소유 목록 ================= */

    // AccessoryID 목록
    UPROPERTY(VisibleAnywhere, Category = "Inventory|Accessories")
    TArray<int32> OwnedAccessoryIDs;

public:
    /* ================= 조회 ================= */

    bool HasAccessory(int32 AccessoryID) const;

    bool IsAccessoryFull() const;

    /* ================= 추가 / 제거 ================= */

    bool TryAddAccessory(int32 AccessoryID);

    bool RemoveAccessory(int32 AccessoryID);

public:
    FOnInventoryChanged OnInventoryChanged;
		
};
