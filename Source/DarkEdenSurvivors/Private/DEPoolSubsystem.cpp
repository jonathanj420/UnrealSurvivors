#include "DEPoolSubsystem.h"
#include "DarkEdenSurvivors.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
void UDEPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    bEnableAutoShrink = true;


    if (bEnableAutoShrink)
    {
        GetWorld()->GetTimerManager().SetTimer(
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
    PooledActorSet.Empty();
    ActorClassMap.Empty();
    Super::Deinitialize();
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
    //SpawnActorDeferred (지연 소환)
    if (!World || !ActorClass) return nullptr;

    // 1. [지연 소환 시작] 아직 월드에 완전히 등록되지 않은 상태로 액터만 만듭니다.
    // BeginPlay도 안 불리고, 물리 엔진에도 등록 안 된 상태입니다.
    AActor* NewActor = World->SpawnActorDeferred<AActor>(
        ActorClass,
        FTransform::Identity, // 위치/회전 (나중에 어차피 다시 잡음)
        nullptr,              // Owner
        nullptr,              // Instigator
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

    if (NewActor)
    {
        // 2. [핵심] 세상에 나오기 전에 미리 충돌과 틱을 꺼버립니다!
        // 이 함수가 실행될 때는 충돌이 꺼진 상태로 설정값만 바뀝니다.
        ActivateActor(NewActor, false);

        // ActivateActor에서는 뺐지만, 여기서만큼은 해줘야 안전하게 풀에 들어갑니다.
        if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(NewActor->GetRootComponent()))
        {
            Root->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        // 3. [소환 완료] 이제 설정된 값(충돌 꺼짐)을 들고 세상에 등장합니다.
        // 이때 컴포넌트가 등록되는데, 이미 NoCollision이라서 오버랩이 안 터집니다.
        UGameplayStatics::FinishSpawningActor(NewActor, FTransform::Identity);
    }

    return NewActor;


}

void UDEPoolSubsystem::ActivateActor(
    AActor* Actor,
    bool bAutoActivate
)
{

    if (!Actor) return;

    // 1. 공통 처리: 켜고 끄기
    Actor->SetActorHiddenInGame(!bAutoActivate);
    Actor->SetActorTickEnabled(bAutoActivate);

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

    //UE_LOG(LogTemp, Warning, TEXT("Shrink Check...")); // 로그 너무 자주 뜨면 성능 저하되니 주석 추천
    for (auto It = PoolMap.CreateIterator(); It; ++It)
    {
        TArray<AActor*>& InactivePool = It.Value();

        // 놀고 있는 놈들이 최소 유지 개수(MinPoolSize)보다 적으면 냅둠
        if (InactivePool.Num() <= MinPoolSize) continue;

        // 초과분 계산
        int32 NumToRemove = InactivePool.Num() - MinPoolSize;

        // 뒤에서부터 삭제 (성능상 이득)
        for (int32 i = 0; i < NumToRemove; ++i)
        {
            // Pop으로 꺼내서
            if (InactivePool.Num() > 0)
            {
                AActor* Victim = InactivePool.Pop();
                PooledActorSet.Remove(Victim);
                if (IsValid(Victim))
                {
                    Victim->Destroy(); // 진짜 삭제
                }
            }
        }
        //UE_LOG(LogTemp, Warning, TEXT("Shrinked %d actors"), NumToRemove);
    }

}

AActor* UDEPoolSubsystem::GetPooledActor(
    TSubclassOf<AActor> ActorClass,
    const FVector& Location,
    const FRotator& Rotation,
    bool bAutoActivate
)
{
    if (!ActorClass) return nullptr;

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    TArray<AActor*>& InactivePool = PoolMap.FindOrAdd(ActorClass);

    AActor* SelectedActor = nullptr;

    // A. 풀에 놀고 있는 놈이 있다 -> 꺼내 쓴다 (Pop)
    while (InactivePool.Num() > 0)
    {
        // 맨 뒤에 있는 놈 하나 꺼냄 (가장 빠름)
        SelectedActor = InactivePool.Pop();
        PooledActorSet.Remove(SelectedActor);
        //UE_LOG(LogTemp, Warning, TEXT("%s got Popped Out from Pool"), *SelectedActor->GetName());
        // 꺼냈는데 그 사이에 죽었거나(Destroy) 유효하지 않으면 버리고 다시 반복
        if (IsValid(SelectedActor))
        {
            break;
        }
        else
        {
            SelectedActor = nullptr;
        }
    }

    // B. 풀이 비었다 -> 새로 만든다
    if (!SelectedActor)
    {
        SelectedActor = CreateNewPooledActor(ActorClass, World);
       // UE_LOG(LogTemp, Warning, TEXT("%s New for Pool"), *SelectedActor->GetName());
    }

    // C. 공통 활성화 처리
    if (SelectedActor)
    {
        // 1. [먼저] 위치와 회전을 확실하게 잡아준다. (텔레포트)
        // Teleport flag를 true로 줘서 물리 엔진 꼬임 방지
        SelectedActor->SetActorLocationAndRotation(Location, Rotation, false, nullptr, ETeleportType::TeleportPhysics);
        //UE_LOG(LogTemp, Warning, TEXT("%s SetLocAndRotFromPool"), *SelectedActor->GetName());
        // 2. [나중] 이제 깨운다. (충돌 ON)
        ActivateActor(SelectedActor, bAutoActivate);
    }

    return SelectedActor;
}

void UDEPoolSubsystem::ReturnActorToPool(AActor* Actor)
{
    if (!IsValid(Actor)) return;

    // 이미 풀에 있는 액터를 또 넣으면 나중에 두 곳에 동시에 꺼내진다
    bool bAlreadyInPool = false;
    PooledActorSet.Add(Actor, &bAlreadyInPool);
    if (bAlreadyInPool)
    {
        UE_LOG(LogDarkEdenSurvivors, Warning, TEXT("[Pool] Duplicate return ignored: %s"), *Actor->GetName());
        return;
    }

    // 1. 끄기
    DeactivateActor(Actor);

    // 2. 풀(창고)에 집어넣기
    TArray<AActor*>& InactivePool = PoolMap.FindOrAdd(Actor->GetClass());
    InactivePool.Add(Actor); // Stack Push
    /*if (!Actor) return;
    DeactivateActor(Actor);*/
}