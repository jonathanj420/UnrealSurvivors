// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DESkillActorBase.h"
#include "DESimpleSummonBase.generated.h"


UCLASS()
class DARKEDENSURVIVORS_API ADESimpleSummonBase : public ADESkillActorBase
{
	GENERATED_BODY()

public:
	ADESimpleSummonBase();
	virtual void Tick(float DeltaTime) override;

	// 풀링 및 초기화 (투사체와 동일)
	virtual void InitializeFromContext(const FDESkillContext& Context);
	virtual void ResetState();
	virtual void ReturnToPool();

protected:
	virtual void BeginPlay() override;





};
