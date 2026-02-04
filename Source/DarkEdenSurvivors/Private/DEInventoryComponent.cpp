// Fill out your copyright notice in the Description page of Project Settings.


#include "DEInventoryComponent.h"
#include "DEAutoSkillBase.h"


// Sets default values for this component's properties
UDEInventoryComponent::UDEInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UDEInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

/* ================= 조회 ================= */

bool UDEInventoryComponent::HasSkill(int32 SkillID) const
{
    return OwnedSkillIDs.Contains(SkillID);
}

bool UDEInventoryComponent::HasAccessory(int32 AccessoryID) const
{
    return OwnedAccessoryIDs.Contains(AccessoryID);
}

bool UDEInventoryComponent::IsSkillFull() const
{
    return OwnedSkillIDs.Num() >= MaxSkillSlots;
}

bool UDEInventoryComponent::IsAccessoryFull() const
{
    return OwnedAccessoryIDs.Num() >= MaxAccessorySlots;
}

/* ================= 추가 ================= */

bool UDEInventoryComponent::TryAddSkill(int32 SkillID)
{
    if (HasSkill(SkillID)) return false;
    if (IsSkillFull()) return false;

    OwnedSkillIDs.Add(SkillID);
    OnInventoryChanged.Broadcast();
    return true;
}

bool UDEInventoryComponent::TryAddAccessory(int32 AccessoryID)
{
    if (HasAccessory(AccessoryID)) return false;
    if (IsAccessoryFull()) return false;

    OwnedAccessoryIDs.Add(AccessoryID);
    OnInventoryChanged.Broadcast();
    return true;
}

/* ================= 제거 ================= */

bool UDEInventoryComponent::RemoveSkill(int32 SkillID)
{
    if (OwnedSkillIDs.Remove(SkillID) > 0)
    {
        OnInventoryChanged.Broadcast();
        return true;
    }
    return false;
}

bool UDEInventoryComponent::RemoveAccessory(int32 AccessoryID)
{
    if (OwnedAccessoryIDs.Remove(AccessoryID) > 0)
    {
        OnInventoryChanged.Broadcast();
        return true;
    }
    return false;
}