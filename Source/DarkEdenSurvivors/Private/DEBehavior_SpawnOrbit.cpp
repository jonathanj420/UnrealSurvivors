// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_SpawnOrbit.h"
#include "DEAOE_OrbitBase.h" 
#include "DEGameInstance.h"
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

    UGameInstance* GI = World->GetGameInstance();
    if (!GI) return;

    UDEPoolSubsystem* Pool = GI->GetSubsystem<UDEPoolSubsystem>();
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

        ADEAOE_OrbitBase* OrbitActor = Cast<ADEAOE_OrbitBase>(RawActor);
        if (!OrbitActor)
        {
            // 이론상 올 일 없음 (안전장치)
            RawActor->Destroy();
            continue;
        }

        // --------------------------------------------------
        // 3. 초기화
        // --------------------------------------------------
        OrbitActor->SetOwner(Instigator);

        // 시작 각도 설정
        OrbitActor->InitOrbit(StartAngle);

        // 데미지 / 범위 / 속도 등 컨텍스트 적용
        OrbitActor->ApplyContext(Context);

        // 활성화 (틱, 충돌 등)
        OrbitActor->ActivateAOE(true);
        //UE_LOG(LogTemp, Warning, TEXT("%s Orbit Activated AOE"), *GetName());
        // 스킬 컨텍스트 관리 목록에 등록
        Context.SpawnedAOEs.Add(OrbitActor);
    }

    //// --------------------------------------------------
    //// 0. 유효성 체크 (SpawnAOE 스타일 유지)
    //// --------------------------------------------------
    //if (Context.AOERequests.Num() == 0) return;

    //AActor* Instigator = Context.Instigator;
    //if (!Instigator) return;

    //UWorld* World = Instigator->GetWorld();
    //if (!World) return;

    //UGameInstance* GI = World->GetGameInstance();
    //if (!GI) return;

    //UDEPoolSubsystem* Pool = GI->GetSubsystem<UDEPoolSubsystem>();
    //if (!Pool) return;

    //// --------------------------------------------------
    //// 1. Orbit 전용 설정 (Amount 확인)
    //// --------------------------------------------------
    //// Context에 있는 Amount (예: 4마리)
    //int32 TotalAmount = FMath::Max(1, Context.Amount);

    //// 각도 쪼개기 (360도 / 4마리 = 90도)
    //float AngleStep = 360.0f / (float)TotalAmount;

    //// --------------------------------------------------
    //// 2. 요청 처리 (Request 루프)
    //// --------------------------------------------------
    //for (const FAOERequest& Request : Context.AOERequests)
    //{
    //    // 클래스 없으면 패스
    //    if (!Request.AOEClass) continue;

    //    // ★ [핵심] Request는 1개지만, Amount만큼 여러 마리 뽑아야 함
    //    for (int32 i = 0; i < TotalAmount; ++i)
    //    {
    //        // 각도 계산 (0도, 90도, 180도...)
    //        float StartAngle = i * AngleStep;

    //        // A. 풀에서 꺼내기
    //        // 위치는 일단 Instigator 위치로 함 (OrbitActor가 Tick에서 알아서 위치 잡음)
    //        AActor* RawActor = Pool->GetPooledActor(
    //            Request.AOEClass,
    //            Instigator->GetActorLocation(),
    //            FRotator::ZeroRotator,
    //            false
    //        );

    //        if (!RawActor) continue;

    //        // B. OrbitBase로 캐스팅 (해골, 성서)
    //        if (ADEAOE_OrbitBase* OrbitActor = Cast<ADEAOE_OrbitBase>(RawActor))
    //        {
    //            // 주인 설정
    //            OrbitActor->SetOwner(Instigator);

    //            // ★ [핵심] 너는 몇 도에서 시작해라!
    //            OrbitActor->InitOrbit(StartAngle);

    //            // C. Context 적용 (데미지, 크기 등)
    //            OrbitActor->ApplyContext(Context);

    //            // D. 활성화 (충돌 켜고, 틱 켜고)
    //            // Orbit은 위치가 계속 바뀌므로 항상 새로 뽑는 느낌으로 true 처리
    //            OrbitActor->ActivateAOE(true);

    //            // E. 관리 목록에 추가
    //            Context.SpawnedAOEs.Add(OrbitActor);
    //        }
    //        else
    //        {
    //            // OrbitBase가 아니면(일반 장판이면) 이 로직을 못 쓰므로 반납
    //            // (안전장치)
    //            if (auto* SimpleAOE = Cast<ADESimpleAOEBase>(RawActor))
    //            {
    //                SimpleAOE->ReturnToPool();
    //            }
    //        }
    //    }
    //}

}
