// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DECharacterBase.h"
#include "DEFemaleVampire.generated.h"

UCLASS()
class DARKEDENSURVIVORS_API ADEFemaleVampire : public ADECharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADEFemaleVampire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
