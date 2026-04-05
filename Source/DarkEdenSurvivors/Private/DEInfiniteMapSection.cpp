// Fill out your copyright notice in the Description page of Project Settings.


#include "DEInfiniteMapSection.h"

// Sets default values
ADEInfiniteMapSection::ADEInfiniteMapSection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCastShadow(false);
	RootComponent = MeshComponent;

	// 1. 사용할 배경 메쉬 로드
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MapMeshAsset(
		TEXT("/Engine/BasicShapes/Plane.Plane")
	);

	if (MapMeshAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MapMeshAsset.Object);
	}

	MeshComponent->SetRelativeScale3D(FVector(20.0f, 20.0f, 1.0f));

	// 3. 충돌 설정
	// 플레이어가 밟고 다녀야 하니까 BlockAll로 설정합니다.
	MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));
}

// Called when the game starts or when spawned
void ADEInfiniteMapSection::BeginPlay()
{
	Super::BeginPlay();
	
	if (MeshComponent->GetStaticMesh())
	{
		FVector MeshExtent = MeshComponent->Bounds.BoxExtent;
		// 정사각형 타일이라고 가정하고 X축 길이를 기준으로 잡습니다.
		MapSize = MeshExtent.X * 2.0f;

		//UE_LOG(LogTemp, Warning, TEXT("MapSection Initialized. Auto-Calculated MapSize: %f"), MapSize);
	}
	else
	{
		// 혹시라도 메쉬 로드 실패 시 기본값 (안전장치)
		MapSize = 2000.0f;
		//UE_LOG(LogTemp, Error, TEXT("MapSection Mesh Missing! Defaulting MapSize to 2000.0f"));
	}

}

// Called every frame
void ADEInfiniteMapSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

