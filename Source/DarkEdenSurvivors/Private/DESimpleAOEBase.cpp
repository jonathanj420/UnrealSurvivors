// Fill out your copyright notice in the Description page of Project Settings.


#include "DESimpleAOEBase.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DEPoolSubsystem.h"
#include "DESkillContext.h"
#include "DEDamageTypes.h"
#include "DEGameplayLibrary.h"
#include "DEAutoSkillBase.h"
#include "DEHealthComponent.h"

ADESimpleAOEBase::ADESimpleAOEBase()
{
    // [중요] 지속 데미지(Tick)를 줘야 하므로 Tick을 켜야 합니다.
    PrimaryActorTick.bCanEverTick = true;


    // [중요] 충돌을 켜야 감지가 됩니다. 'OverlapAllDynamic' 추천 (Pawn, Enemy 등 감지)
    CollisionComponent->SetCollisionProfileName(TEXT("PlayerAttack"));
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ADESimpleAOEBase::OnOverlapBegin);
}

void ADESimpleAOEBase::BeginPlay()
{
    Super::BeginPlay();

    // 동적 델리게이트 바인딩 (들어오자마자 때리기 위해)
    
}


void ADESimpleAOEBase::InitializeFromContext(const FDESkillContext& Context)
{
    // 2. 장판(AOE) 전용 데이터 세팅
    HitCooldown = Context.GetValue(TEXT("HitInterval"), 0.5f);
    //UE_LOG(LogTemp, Warning, TEXT("%s Try Init from ContXXt"), *GetName());
    // 1. 공통 스탯(데미지, 범위, 오너, 수명, 풀링 리셋 등)은 부모에게 짬처리!
    Super::InitializeFromContext(Context);

   

    // 3. 영구 지속 장판 처리 (독 장판, 마늘 등)
    if (LifeTime < 0.0f)
    {
        bInfiniteDuration = true;
        // 부모가 켜둔 수명 타이머를 장판은 강제로 끕니다 (영원히 안 죽음)
        GetWorldTimerManager().ClearTimer(LifeTimeTimerHandle);
    }
    else
    {
        bInfiniteDuration = false;
    }

    // 4. (선택) 나이아가라 크기 자동 조절 연동
    if (NiagaraComponent)
    {
        // 부모가 세팅해둔 Size(또는 Context.Radius) 활용
        NiagaraComponent->SetFloatParameter(FName("SkillRadius"), Context.Radius);
    }

    CollisionComponent->SetSphereRadius(Radius);


    //// 1. 상태 리셋 (풀링 재사용 시 필수)
    //// (InitializeAOE 안에서 HitCooldownMap.Empty()를 하긴 하지만, 명시적으로 초기화 로직이 있다면 여기서 호출)
    //ResetState(); 

    //// 2. 데이터 추출
    //// 데미지, 범위, 지속시간은 컨텍스트의 기본 스탯 사용
    //CachedContext = Context;
    //float FinalDamage = Context.Damage;
    //float FinalRadius = Context.Radius; // 혹은 Context.Range 등 스탯 이름에 맞춰 사용
    //float FinalDuration = Context.Duration;

    //// [중요] 타격 간격(HitCooldown) 결정 로직
    //// 방법 A: 컨텍스트 Map에 "HitInterval"이라는 값이 있으면 쓰고, 없으면 0.5초 기본값
    //float FinalHitCooldown = Context.GetValue(TEXT("HitInterval"), 0.5f);

    //// 방법 B: (선택사항) 공격 속도(Speed) 스탯이 높으면 타격 간격이 짧아지게 하려면?
    //// if (Context.Speed > 0.f) FinalHitCooldown = 1.0f / Context.Speed;

    //// 3. 메인 초기화 함수 호출
    //InitializeAOE(FinalDamage, FinalRadius, FinalHitCooldown, FinalDuration);

    //// 4. 확장 스탯 적용 (Map에서 꺼내오기)
    //// 넉백 (Knockback)
    //float KForce = Context.KnockbackForce; // 혹은 Context.GetValue("Knockback")
    //if (KForce > 0.f)
    //{
    //    // 넉백 힘을 멤버 변수에 저장하거나 설정 (멤버 변수가 있다고 가정)
    //    // KnockbackForce = KForce; 
    //}

    //// 크기 보정 (Radius와 별개로 비주얼만 키우고 싶을 때)
    //float VisualScale = Context.GetValue(TEXT("Scale"), 1.0f);
    //if (VisualScale != 1.0f)
    //{
    //    SetActorScale3D(FVector(VisualScale));
    //}
}

void ADESimpleAOEBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
    // 나 자신이나 주인은 무시
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;
    // 3. [핵심] 몬스터의 "핵심 부위(Root)"가 아니면 무시! (팔, 다리, 메쉬 충돌 방지)
    if (OtherComp != OtherActor->GetRootComponent())
    {
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("%s OnOverlapBegin"), *GetName());
    // 들어오자마자 쿨타임 체크 후 즉시 타격 (반응성 UP)
    if (CanHitTarget(OtherActor))
    {
        OnHitTarget(OtherActor);
        // 다음 타격 시간 기록 but y here? ?
        /*double CurrentTime = GetWorld()->GetTimeSeconds();
        HitCooldownMap.Add(OtherActor, CurrentTime + HitCooldown);*/
    }
}

void ADESimpleAOEBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 현재 겹쳐있는 모든 액터 가져오기
    TArray<AActor*> OverlappingActors;
    CollisionComponent->GetOverlappingActors(OverlappingActors);

    const double CurrentTime = GetWorld()->GetTimeSeconds();

    for (AActor* Target : OverlappingActors)
    {
        if (!Target || Target == GetOwner()) continue;

        // 쿨타임 지났는지 확인
        if (CanHitTarget(Target))
        {
            OnHitTarget(Target);
            UE_LOG(LogTemp, Warning, TEXT("OnHitTarget to : %s Called in Tick"), *Target->GetName());
            HitCooldownMap.Add(Target, CurrentTime + HitCooldown);
        }
    }

    // [최적화 팁] 
    // HitCooldownMap에 있지만 현재 OverlappingActors에 없는(범위 밖으로 나간) 
    // 적들은 Map에서 제거해주면 메모리를 아낄 수 있습니다. 
    // 하지만 뱀서류는 적이 금방 죽으므로 굳이 안 해도 무방합니다.
}

bool ADESimpleAOEBase::CanHitTarget(AActor* Target) const
{
    if (!Target) return false;

    const double CurrentTime = GetWorld()->GetTimeSeconds();

    // 맵에 기록이 있다면, 시간 체크
    if (const double* NextHitTime = HitCooldownMap.Find(Target))
    {
        //UE_LOG(LogTemp, Warning, TEXT("%s CHECKED CanHitTarget, Target already exist"),*GetName());
        return CurrentTime >= *NextHitTime;
    }

    // 기록이 없으면(처음 만남) 즉시 타격 가능
    //UE_LOG(LogTemp, Warning, TEXT("%s CHECKED CanHitTarget, New Target Hit immediately"), *GetName());
    return true;
}

void ADESimpleAOEBase::OnHitTarget(AActor* Target)
{
    double CurrentTime = GetWorld()->GetTimeSeconds();
    HitCooldownMap.Add(Target, CurrentTime + HitCooldown);
    TryDealDamage(Target);
    UE_LOG(LogTemp, Warning, TEXT("AOE %s Hit : %s"), *GetName(), *Target->GetName());
    //UE_LOG(LogTemp, Warning, TEXT("%s got hit by : %s"), *Target->GetName(), *GetName());
    // [확장 포인트]
    // 여기서 넉백(Knockback)이나 상태이상(ApplyStatus) 로직을 추가하면 됩니다.
    // 예: ApplyKnockback(Target);
}


void ADESimpleAOEBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 혹시라도 Destroy로 삭제될 때 타이머나 바인딩 정리
    CollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ADESimpleAOEBase::OnOverlapBegin);
    Super::EndPlay(EndPlayReason);
}

void ADESimpleAOEBase::ResetState()
{
    
    Super::ResetState();

    // 맵 초기화
    
}

void ADESimpleAOEBase::ReturnToPool()
{
    Super::ReturnToPool();
    HitCooldownMap.Empty();
    HitCountMap.Empty();
}
