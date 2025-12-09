#include "DEPoolSubsystem.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

void UDEPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    bEnableAutoShrink = true;


    if (bEnableAutoShrink)
    {
        GetWorldSafe()->GetTimerManager().SetTimer(
            ShrinkTimerHandle,
            this,
            &UDEPoolSubsystem::ShrinkPools,
            ShrinkCheckInterval,
            true
        );
    }
}

void UDEPoolSubsystem::Deinitialize()
{
    // 풀 내 모든 액터 삭제
    for (auto& Pair : PoolMap)
    {
        for (AActor* Actor : Pair.Value)
        {
            if (IsValid(Actor))
            {
                Actor->Destroy();
            }
        }
    }

    PoolMap.Empty();
    ActorClassMap.Empty();
    Super::Deinitialize();
}

UWorld* UDEPoolSubsystem::GetWorldSafe() const
{
    if (!GetGameInstance()) return nullptr;
    return GetGameInstance()->GetWorld();
}

bool UDEPoolSubsystem::IsActorInactive(AActor* Actor) const
{
    if (!IsValid(Actor))
        return false;

    const bool bHidden = Actor->IsHidden();
    const bool bTickOff = !Actor->IsActorTickEnabled();

    bool bCollisionOff = true;
    if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
    {
        bCollisionOff = Root->GetCollisionEnabled() == ECollisionEnabled::NoCollision;
    }

    return bHidden && bTickOff && bCollisionOff;
}

AActor* UDEPoolSubsystem::CreateNewPooledActor(TSubclassOf<AActor> ActorClass, UWorld* World)
{
    if (!World || !ActorClass)
        return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* NewActor = World->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
    if (NewActor)
    {
        DeactivateActor(NewActor);  // 생성 후 기본은 비활성 상태
    }

    return NewActor;
}

void UDEPoolSubsystem::ActivateActor(
    AActor* Actor,
    const FVector& Location,
    const FRotator& Rotation,
    bool bAutoActivate
)
{
    if (!Actor) return;

    Actor->SetActorLocation(Location);
    Actor->SetActorRotation(Rotation);

    Actor->SetActorHiddenInGame(!bAutoActivate);
    Actor->SetActorTickEnabled(bAutoActivate);

    if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
    {
        Root->SetCollisionEnabled(
            bAutoActivate ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision
        );
    }
}

void UDEPoolSubsystem::DeactivateActor(AActor* Actor)
{
    if (!Actor) return;

    Actor->SetActorHiddenInGame(true);
    Actor->SetActorTickEnabled(false);

    if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
    {
        Root->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void UDEPoolSubsystem::ShrinkPools()
{
    UE_LOG(LogTemp, Warning, TEXT("Try Shrink Pool"));
    for (auto& Elem : PoolMap)
    {
        TArray<AActor*>& Pool = Elem.Value;

        // 1) 비활성 액터만 수집
        TArray<AActor*> InactiveActors;
        for (AActor* A : Pool)
        {
            if (IsActorInactive(A))
                InactiveActors.Add(A);
        }

        // 최소 유지 개수보다 작으면 안 줄임
        if (InactiveActors.Num() <= MinPoolSize)
            continue;

        int32 ToRemove = InactiveActors.Num() - MinPoolSize;

        // 2) 초과분 Destroy
        for (int32 i = 0; i < ToRemove; i++)
        {
            AActor* A = InactiveActors[i];
            if (A && !A->IsActorInitialized()) continue;

            Pool.Remove(A);
            ActorClassMap.Remove(A);
            A->Destroy();
        }
        UE_LOG(LogTemp, Warning, TEXT("%d Actors Shrinked"),ToRemove);
    }

}

AActor* UDEPoolSubsystem::GetPooledActor(
    TSubclassOf<AActor> ActorClass,
    const FVector& Location,
    const FRotator& Rotation,
    bool bAutoActivate
)
{
    if (!ActorClass)
        return nullptr;

    UWorld* World = GetWorldSafe();
    if (!World)
        return nullptr;

    TArray<AActor*>& Pool = PoolMap.FindOrAdd(ActorClass);

    // 1) 비활성 액터 재사용
    for (AActor* Actor : Pool)
    {
        if (IsActorInactive(Actor))
        {
            ActivateActor(Actor, Location, Rotation, bAutoActivate);
            return Actor;
        }
    }

    // 2) 없으면 새로 생성
    AActor* NewActor = CreateNewPooledActor(ActorClass, World);
    if (NewActor)
    {
        Pool.Add(NewActor);
        ActorClassMap.Add(NewActor, ActorClass);
        ActivateActor(NewActor, Location, Rotation, bAutoActivate);
        return NewActor;
    }

    return nullptr;
}

void UDEPoolSubsystem::ReturnActorToPool(AActor* Actor)
{
    if (!Actor) return;
    DeactivateActor(Actor);
}