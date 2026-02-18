// Fill out your copyright notice in the Description page of Project Settings.


#include "DELevelUpManagerComponent.h"
#include "DELevelUpChoice_Skill.h"
#include "DELevelUpChoice_Accessory.h"
#include "DESkillManagerComponent.h"
#include "DEAccessoryData.h"
#include "Kismet/KismetMathLibrary.h"

UDELevelUpManagerComponent::UDELevelUpManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDELevelUpManagerComponent::BeginPlay()
{
    Super::BeginPlay();
}

TArray<UDELevelUpChoiceBase*> UDELevelUpManagerComponent::GenerateChoices(int32 Count)
{
    TArray<UDELevelUpChoiceBase*> Result;

    for (int32 i = 0; i < Count; ++i)
    {
        float Roll = FMath::FRand();

        if (Roll < SkillWeight)
        {
            if (UDELevelUpChoiceBase* Choice = CreateRandomSkillChoice())
            {
                Result.Add(Choice);
                UE_LOG(LogTemp, Warning, TEXT("Random Choice : %s"), *Choice->GetName());
            }
        }
        else
        {
            if (UDELevelUpChoiceBase* Choice = CreateRandomAccessoryChoice())
            {
                Result.Add(Choice);
                UE_LOG(LogTemp, Warning, TEXT("Random Choice : %s"), *Choice->GetName());
            }
        }
    }

    return Result;
}

void UDELevelUpManagerComponent::ApplyChoice(UDELevelUpChoiceBase* Choice)
{
    if (!Choice)
        return;

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
        return;

    // ★ 핵심: 실제 적용은 Choice가 한다
    Choice->Apply(OwnerActor);
}

UDELevelUpChoiceBase* UDELevelUpManagerComponent::CreateRandomSkillChoice()
{
    UDESkillManagerComponent* SkillMgr =
        GetOwner()->FindComponentByClass<UDESkillManagerComponent>();

    if (!SkillMgr)
        return nullptr;

    const FDESkillRow* Row = SkillMgr->GetRandomSkillRow();
    if (!Row)
        return nullptr;

    UDELevelUpChoice_Skill* Choice =
        NewObject<UDELevelUpChoice_Skill>(this);

    Choice->Init(
        Row->SkillID,
        Row->SkillName,
        Row->SkillIcon
    );

    return Choice;
}

UDELevelUpChoiceBase* UDELevelUpManagerComponent::CreateRandomAccessoryChoice()
{
    if (AccessoryPool.Num() == 0)
        return nullptr;

    int32 Index = FMath::RandRange(0, AccessoryPool.Num() - 1);

    UDELevelUpChoice_Accessory* Choice =
        NewObject<UDELevelUpChoice_Accessory>(this);

    Choice->Init(AccessoryPool[Index]);

    return Choice;
}