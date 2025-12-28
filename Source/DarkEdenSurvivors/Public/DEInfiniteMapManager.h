// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DEInfiniteMapManager.generated.h"

UCLASS()
class DARKEDENSURVIVORS_API ADEInfiniteMapManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADEInfiniteMapManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// 생성할 타일 클래스 (블루프린트로 만든 타일 BP를 여기에 넣으세요)
	UPROPERTY(EditAnywhere, Category = "Map Setting")
	TSubclassOf<class ADEInfiniteMapSection> MapSectionClass;

	UPROPERTY(EditAnywhere, Category = "Map Setting", meta = (ClampMin = "2"))
	int32 GridSize = 3;
	// 타일 하나의 크기 (첫 타일 생성 시 자동 측정)
	float TileSize = 0.0f;
	// 관리할 타일 목록
	UPROPERTY(VisibleAnywhere, Category = "Map")
	TArray<TObjectPtr<class ADEInfiniteMapSection>> MapSections;

	// 업데이트 주기 (타이머)
	void UpdateMapSections();
	FTimerHandle MapUpdateTimerHandle;
};
