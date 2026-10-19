// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SpawnOrbit.h"
#include "DEAOE_OrbitBase.h" 
#include "DEAutoSkillBase.h"
#include "DEPoolSubsystem.h"
#include "DESkillContext.h"

void UDEBehavior_SpawnOrbit::Execute(FDESkillContext& Context)
{
    // --------------------------------------------------
   // 0. 유효성 체크
   // --------------------------------------------------
    if (!OrbitAOEClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnOrbit: OrbitAOEClass is null"));
        return;
    }

    AActor* Instigator = Context.Instigator;
    if (!Instigator) return;

    UWorld* World = Instigator->GetWorld();
    if (!World) return;

    UDEPoolSubsystem* Pool = World->GetSubsystem<UDEPoolSubsystem>();
    if (!Pool) return;

    // --------------------------------------------------
    // 1. Orbit 개수 & 각도 계산
    // --------------------------------------------------
    const int32 TotalAmount = FMath::Max(1, Context.Amount);
    const float AngleStep = 360.0f / static_cast<float>(TotalAmount);

    // --------------------------------------------------
    // 2. Orbit AOE 스폰
    // --------------------------------------------------
    for (int32 i = 0; i < TotalAmount; ++i)
    {
        const float StartAngle = i * AngleStep;
        //Radius만큼의 Vector를 FRotator(각도)만큼.RorateVector()함 
        FVector OwnerLoc = Instigator->GetActorLocation();
        FVector Offset = FRotator(0.f, StartAngle, 0.f).RotateVector(FVector(Context.GetValue(TEXT("OrbitRadius"), 150.0f), 0.f, 0.f));
        FVector SpawnLocation = OwnerLoc + Offset;

        // 풀에서 Orbit AOE 꺼내기 > 수정 : 원점(플레이어)에서 스폰하면 스폰하자마자 플레이어랑 겹친놈 때림
        //위에서 계산한 context radius(반지름),startangle을 기준으로 SpawnLocation 스폰 
        AActor* RawActor = Pool->GetPooledActor(
            OrbitAOEClass,
            SpawnLocation,
            FRotator::ZeroRotator,
            false
        );
        //UE_LOG(LogTemp, Warning, TEXT("%s SpawnOrbit for Instigator Location"), *RawActor->GetName());
        if (!RawActor) continue;

        // OrbitAOEClass가 ADEAOE_OrbitBase 계열이라 실패할 일은 없다
        ADEAOE_OrbitBase* OrbitActor = Cast<ADEAOE_OrbitBase>(RawActor);
        if (!OrbitActor) continue;

        // --------------------------------------------------
        // 3. 초기화
        // --------------------------------------------------
        OrbitActor->SetOwner(Instigator);

        // 시작 각도 설정
        OrbitActor->InitOrbit(StartAngle);
        OrbitActor->InitializeFromContext(Context);

        //// 데미지 / 범위 / 속도 등 컨텍스트 적용
        //OrbitActor->ApplyContext(Context);

        //// 활성화 (틱, 충돌 등)
        //OrbitActor->ActivateAOE(true);
        // 
        //UE_LOG(LogTemp, Warning, TEXT("%s Orbit Activated AOE"), *GetName());
        // 수명이 없는 궤도는 CancelSkill이 거두도록 스킬에 등록
        if (Context.SourceSkill)
        {
            Context.SourceSkill->RegisterSkillActor(OrbitActor);
        }
    }

}
