#include "DEPoolSubsystem.h"
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
    ActorClassMap.Empty();
    Super::Deinitialize();
}

//UWorld* UDEPoolSubsystem::GetWorldSafe() const
//{
//    if (!GetGameInstance()) return nullptr;
//    return GetGameInstance()->GetWorld();
//}

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

    /*if (!World || !ActorClass) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* NewActor = World->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);

    if (NewActor)
    {
        ActivateActor(NewActor, false);
    }
    return NewActor;*/


    //if (!World || !ActorClass)
    //    return nullptr;

    //FActorSpawnParameters Params;
    //Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    //AActor* NewActor = World->SpawnActor<AActor>(ActorClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
    //if (NewActor)
    //{
    //    DeactivateActor(NewActor);  // 생성 후 기본은 비활성 상태
    //}

    //return NewActor;
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

    // off this shi would be better i guess
    //// 2. 충돌 처리 (여전히 QueryOnly 강제인 건 나중에 인터페이스로 빼야 함)
    //if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
    //{
    //    Root->SetCollisionEnabled(
    //        bAutoActivate ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision
    //    );
    //    if (bAutoActivate)
    //    {
    //       // UE_LOG(LogTemp, Warning, TEXT("%s Activated for True"), *Actor->GetName());
    //    }
    //    else
    //    {
    //        //UE_LOG(LogTemp, Warning, TEXT("%s Activated for False"), *Actor->GetName());
    //    }
    //}

    // 3. 만약 인터페이스를 쓴다면 여기서 호출 (위치 인자 없이)
    // if (Actor->Implements<UDEPoolableInterface>()) ...

    /*if (!Actor) return;

    Actor->SetActorLocation(Location);
    Actor->SetActorRotation(Rotation);

    Actor->SetActorHiddenInGame(!bAutoActivate);
    Actor->SetActorTickEnabled(bAutoActivate);

    if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
    {
        Root->SetCollisionEnabled(
            bAutoActivate ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision
        );
    }*/
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
                if (IsValid(Victim))
                {
                    Victim->Destroy(); // 진짜 삭제
                }
            }
        }
        //UE_LOG(LogTemp, Warning, TEXT("Shrinked %d actors"), NumToRemove);
    }

    //UE_LOG(LogTemp, Warning, TEXT("Try Shrink Pool"));
    //for (auto& Elem : PoolMap)
    //{
    //    TArray<AActor*>& Pool = Elem.Value;

    //    // 1) 비활성 액터만 수집
    //    TArray<AActor*> InactiveActors;
    //    for (AActor* A : Pool)
    //    {
    //        if (IsActorInactive(A))
    //            InactiveActors.Add(A);
    //    }

    //    // 최소 유지 개수보다 작으면 안 줄임
    //    if (InactiveActors.Num() <= MinPoolSize)
    //        continue;

    //    int32 ToRemove = InactiveActors.Num() - MinPoolSize;

    //    // 2) 초과분 Destroy
    //    for (int32 i = 0; i < ToRemove; i++)
    //    {
    //        AActor* A = InactiveActors[i];
    //        if (A && !A->IsActorInitialized()) continue;

    //        Pool.Remove(A);
    //        ActorClassMap.Remove(A);
    //        A->Destroy();
    //    }
    //    UE_LOG(LogTemp, Warning, TEXT("%d Actors Shrinked"),ToRemove);
    //}

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
    //if (!ActorClass)
    //    return nullptr;

    //UWorld* World = GetWorldSafe();
    //if (!World)
    //    return nullptr;

    //TArray<AActor*>& Pool = PoolMap.FindOrAdd(ActorClass);

    //// 1) 비활성 액터 재사용
    //for (AActor* Actor : Pool)
    //{
    //    if (IsActorInactive(Actor))
    //    {
    //        ActivateActor(Actor, Location, Rotation, bAutoActivate);
    //        return Actor;
    //    }
    //}

    //// 2) 없으면 새로 생성
    //AActor* NewActor = CreateNewPooledActor(ActorClass, World);
    //if (NewActor)
    //{
    //    Pool.Add(NewActor);
    //    ActorClassMap.Add(NewActor, ActorClass);
    //    ActivateActor(NewActor, Location, Rotation, bAutoActivate);
    //    return NewActor;
    //}

    //return nullptr;
}

void UDEPoolSubsystem::ReturnActorToPool(AActor* Actor)
{
    if (!IsValid(Actor)) return;

    // 1. 끄기
    DeactivateActor(Actor);

    // 2. 풀(창고)에 집어넣기
    // [주의] 이미 풀에 있는데 또 넣는 실수를 방지하기 위해 AddUnique 대신 로직으로 관리하거나
    // 성능을 위해 그냥 Add를 하되, 이 함수는 반드시 "사용 중인 놈"한테만 호출해야 함.
    TArray<AActor*>& InactivePool = PoolMap.FindOrAdd(Actor->GetClass());
    InactivePool.Add(Actor); // Stack Push
    /*if (!Actor) return;
    DeactivateActor(Actor);*/
}