// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DEItemBase.generated.h"

UCLASS()
class DARKEDENSURVIVORS_API ADEItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADEItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere)
    class USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* Mesh;

    UFUNCTION()
    virtual void OnOverlap(
        UPrimitiveComponent* Overlapped,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

public:
    virtual void ApplyEffect(class ADEFemaleVampire* Player);

    virtual void ActivateItem(const FVector& Location);
    virtual void DeactivateItem();
    virtual void ResetItem();

};
