// Fill out your copyright notice in the Description page of Project Settings.


#include "DEInfiniteMapManager.h"
#include "DEInfiniteMapSection.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values
ADEInfiniteMapManager::ADEInfiniteMapManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    MapSectionClass = ADEInfiniteMapSection::StaticClass();
}

// Called when the game starts or when spawned
void ADEInfiniteMapManager::BeginPlay()
{
    Super::BeginPlay();

    if (!MapSectionClass)
    {
        UE_LOG(LogTemp, Error, TEXT("MapSectionClass is NOT set in MapManager!"));
        return;
    }

    // 1. 타일 자동 생성
    // 플레이어 위치를 기준으로 생성해도 되지만, 일단 (0,0) 주변에 깔아두면
    // 어차피 Update 로직이 바로 플레이어 주변으로 끌고 옵니다.

    const int32 HalfSize = GridSize / 2; // 예: 3이면 1, 4면 2
    // 초기 생성 시 임시 위치 계산을 위해 MapSize가 필요한데, 
    // 아직 생성이 안 돼서 모름. 일단 0으로 뭉쳐서 생성하고 첫 놈한테 물어봄.

    for (int32 i = 0; i < GridSize * GridSize; i++)
    {
        FVector SpawnLocation(0, 0, -100.0f); // 일단 대충 아래에 생성
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        ADEInfiniteMapSection* NewSection = GetWorld()->SpawnActor<ADEInfiniteMapSection>(
            MapSectionClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams
        );

        if (NewSection)
        {
            MapSections.Add(NewSection);

            // 첫 타일이면 사이즈를 측정해서 저장
            if (i == 0)
            {
                TileSize = NewSection->GetMapSize();
                if (TileSize <= 0.0f) TileSize = 2000.0f; // 안전장치
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("MapManager Spawned %d Tiles. TileSize: %f"), MapSections.Num(), TileSize);

    // 2. 초기 배치 정렬 (바둑판 모양으로 예쁘게 펴주기)
    // 이걸 안 하면 한 점에 뭉쳐 있다가 0.2초 뒤에 팍 퍼지는데, 보기에 안 좋을 수 있음.
    int32 Index = 0;
    for (int32 X = 0; X < GridSize; X++)
    {
        for (int32 Y = 0; Y < GridSize; Y++)
        {
            if (MapSections.IsValidIndex(Index))
            {
                // (X, Y) 인덱스를 월드 좌표로 변환 (중심 기준)
                float PosX = (X - HalfSize) * TileSize;
                float PosY = (Y - HalfSize) * TileSize;

                // 플레이어 초기 위치가 있다면 그 주변에 생성하는 게 좋음
                // (여기서는 그냥 월드 원점 기준 배치)
                MapSections[Index]->SetActorLocation(FVector(PosX, PosY, 0.0f));
                Index++;
            }
        }
    }

    // 3. 타이머 시작 (0.2초마다 검사)
    GetWorld()->GetTimerManager().SetTimer(
        MapUpdateTimerHandle,
        this,
        &ADEInfiniteMapManager::UpdateMapSections,
        0.2f,
        true
    );
}

// Called every frame
void ADEInfiniteMapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADEInfiniteMapManager::UpdateMapSections()
{
    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Player || MapSections.Num() == 0 || TileSize <= 0.0f) return;

    FVector PlayerLocation = Player->GetActorLocation();

    // [중요] 전체 맵의 너비/높이 계산
    // 3x3이고 타일이 2000이면, 전체 커버 범위는 6000이 아님.
    // 타일이 점프해야 할 거리는 'TileSize * GridSize' 입니다.
    float MapTotalWidth = TileSize * GridSize;
    float HalfTotalWidth = MapTotalWidth * 0.5f;

    for (ADEInfiniteMapSection* Section : MapSections)
    {
        if (!Section) continue;

        FVector SectLocation = Section->GetActorLocation();
        FVector NewLocation = SectLocation;

        // 공식 일반화: 플레이어와 타일의 거리가 '전체 폭의 절반'보다 멀어지면
        // 반대편으로 '전체 폭(MapTotalWidth)'만큼 순간이동 시킨다.

        // X축 (위/아래)
        if (PlayerLocation.X - SectLocation.X > HalfTotalWidth)
        {
            NewLocation.X += MapTotalWidth;
        }
        else if (PlayerLocation.X - SectLocation.X < -HalfTotalWidth)
        {
            NewLocation.X -= MapTotalWidth;
        }

        // Y축 (좌/우)
        if (PlayerLocation.Y - SectLocation.Y > HalfTotalWidth)
        {
            NewLocation.Y += MapTotalWidth;
        }
        else if (PlayerLocation.Y - SectLocation.Y < -HalfTotalWidth)
        {
            NewLocation.Y -= MapTotalWidth;
        }

        if (NewLocation != SectLocation)
        {
            Section->SetActorLocation(NewLocation);
        }
    }
}