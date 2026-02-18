// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DELevelUpManagerComponent.generated.h"

class UDELevelUpChoiceBase;
class UDEAccessoryData;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DARKEDENSURVIVORS_API UDELevelUpManagerComponent : public UActorComponent
{
    GENERATED_BODY()

    public:
        UDELevelUpManagerComponent();

        TArray<UDELevelUpChoiceBase*> GenerateChoices(int32 Count);
        void ApplyChoice(UDELevelUpChoiceBase* Choice);
    protected:
        virtual void BeginPlay() override;

    private:
        // ===== 설정값 =====

        // 악세서리 후보들 (에디터에서 등록)
        UPROPERTY(EditAnywhere, Category = "LevelUp")
        TArray<UDEAccessoryData*> AccessoryPool;

        // 스킬 확률 (0~1)
        UPROPERTY(EditAnywhere, Category = "LevelUp")
        float SkillWeight = 0.7f;

        UPROPERTY(EditAnywhere, Category = "LevelUp")
        float AccessoryWeight = 0.3f;

    private:
        UDELevelUpChoiceBase* CreateRandomSkillChoice();
        UDELevelUpChoiceBase* CreateRandomAccessoryChoice();


};
