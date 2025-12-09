// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TestAIController.h"
#include "TestStatComponent.h"
#include "TestPawn.generated.h"

UCLASS()
class DARKEDENSURVIVORS_API ATestPawn : public APawn
{
    GENERATED_BODY()

public:
    ATestPawn();
    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

protected:
    // **1. 루트 컴포넌트 (충돌체)**: 폰이 월드에 존재하기 위한 필수 요소
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCapsuleComponent* CapsuleComponent;
    //class USphereComponent* RootSphere; // Forward Declaration 대신 class 키워드 사용??
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* MeshComponent;

    // **2. 이동 컴포넌트**
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    UFloatingPawnMovement* MovementComponent;

    // **3. 속성/스탯 컴포넌트**
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    UTestStatComponent* TestStatComponent;
    
    APawn* TargetPlayer;

    UPROPERTY(VisibleAnywhere, Category = "Move")
    FVector Dir;
public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

};
