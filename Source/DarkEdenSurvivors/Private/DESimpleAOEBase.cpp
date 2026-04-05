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

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;

    // [중요] 충돌을 켜야 감지가 됩니다. 'OverlapAllDynamic' 추천 (Pawn, Enemy 등 감지)
    Collision->SetCollisionProfileName(TEXT("Projectile"));
    //Collision->SetGenerateOverlapEvents(true); // 이벤트 발생 필수

    NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
    NiagaraComp->SetupAttachment(RootComponent);
    // 나이아가라는 충돌할 필요 없음
    NiagaraComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADESimpleAOEBase::BeginPlay()
{
    Super::BeginPlay();

    // 동적 델리게이트 바인딩 (들어오자마자 때리기 위해)
    Collision->OnComponentBeginOverlap.AddDynamic(this, &ADESimpleAOEBase::OnOverlapBegin);
}

void ADESimpleAOEBase::ApplyContext(const FDESkillContext& Context)
{
    //피해 주체 설정 (Player / Monster 공통)
    DamageInstigator = Context.Instigator;
    CachedContext = Context;
    //Context에서 기본 스탯 추출
    const float FinalDamage = Context.Damage;
    const float FinalRadius = Context.Radius;
    const float FinalDuration = Context.Duration;
    CritChance = Context.CritChance;
    CritDamageMultiplier = Context.CritDamageMultiplier;
    KnockbackForce = Context.KnockbackForce;
    Snapshot = Context.FinalSnapshot;
    //타격 간격 (확장 가능)
    const float FinalHitCooldown =
        Context.GetValue(TEXT("HitInterval"), 0.5f);
    OwnerActor = Context.Instigator;
    //내부 상태 초기화 + 반영
    InitializeAOE(
        FinalDamage,
        FinalRadius,
        FinalHitCooldown,
        FinalDuration
    );

    //비주얼 전용 보정 (선택)
    const float VisualScale =
        Context.GetValue(TEXT("Scale"), 1.0f);

    if (!FMath::IsNearlyEqual(VisualScale, 1.0f))
    {
        SetActorScale3D(FVector(VisualScale));
    }
}

void ADESimpleAOEBase::ActivateAOE(bool bIsNewSpawn)
{
    //UE_LOG(LogTemp, Warning, TEXT("%s Starting Activation . . ."), *GetName());
    // 1. 하드웨어 세팅 (이건 새것이든 헌것이든 무조건 켜져야 함)
    SetActorHiddenInGame(false);
    SetActorTickEnabled(true);
    if (Collision)
    {
        //UE_LOG(LogTemp, Warning, TEXT("%s Setting Collision : QueryOnly"), *GetName());
        Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        
    }

    // 2. [핵심] 소프트웨어 리셋 (조건부 초기화)
    if (bIsNewSpawn)
    {
        //// 진짜 새로 태어난 놈만 기억 소거
        //HitCooldownMap.Empty();

        //UE_LOG(LogTemp, Warning, TEXT("%s has got hitcooldown map cleared"),*GetName());
        //// 이펙트도 아예 처음부터 다시 터트림
        //if (NiagaraComp) NiagaraComp->Activate(true);
        //UE_LOG(LogTemp, Warning, TEXT("%s is NEWBORN"), *GetName());
    }
    else
    {
        // [예외 처리] 영구 지속(Persistent) 스킬이 재사용된 경우
        // 1. 쿨타임 맵(HitCooldownMap)을 비우지 않음 -> 방금 때린 놈 또 때리는 '따닥' 방지
        // 2. 이펙트는 굳이 껐다 켜지 않고 유지 (자연스럽게)

        // (선택사항) 만약 레벨업 효과를 주고 싶다면 여기서 스케일이나 컬러만 살짝 변경
    }

    //// 1. 엔진 기능 켜기 (PoolSubsystem이 해주는 거랑 비슷함)
    //SetActorHiddenInGame(false);
    //SetActorTickEnabled(true);
    //Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    //// ---------------------------------------------
    //// [중요] PoolSubsystem은 절대 모르는 "내 전용 로직" 리셋
    //// ---------------------------------------------

    //// 2. 이펙트 다시 재생 (필수!)
    //if (NiagaraComp)
    //{
    //    NiagaraComp->Activate(true);
    //}

    //// 3. 내부 상태 리셋 (필수!)
    //// InitializeFromContext에서 하긴 하지만, 확실하게 여기서 한 번 더 비워주는 게 안전함
    //HitCooldownMap.Empty();
    //HitCountMap.Empty();
}

void ADESimpleAOEBase::InitializeAOE(float InDamage, float InRadius, float InHitCooldown, float InLifeTime)
{
    Damage = InDamage;
    HitCooldown = InHitCooldown;
    LifeTime = InLifeTime;
    //OwnerActor = InOwner;
    bInfiniteDuration = (LifeTime <= 0.f);

    // 범위 설정
    Collision->SetSphereRadius(InRadius);

    // [옵션] 나이아가라 크기도 반경에 맞춰 자동 조절 (예: 300단위가 스케일 1.0일 때)
    // float EffectScale = InRadius / 300.0f;
    if (NiagaraComp)
    {
        NiagaraComp->SetFloatParameter(FName("SkillRadius"), InRadius);
    }

    // 맵 초기화 (재사용 시 필수)
    //HitCooldownMap.Empty();

    // 수명 설정 (0보다 크면 시간 뒤 자동 삭제/반환)
    if (!bInfiniteDuration)
    {
        SetLifeSpan(LifeTime);
    }
    else
    {
        SetLifeSpan(0); // 무한
    }
}

void ADESimpleAOEBase::InitializeFromContext(const FDESkillContext& Context)
{
    // 1. 상태 리셋 (풀링 재사용 시 필수)
    // (InitializeAOE 안에서 HitCooldownMap.Empty()를 하긴 하지만, 명시적으로 초기화 로직이 있다면 여기서 호출)
    // ResetState(); 

    // 2. 데이터 추출
    // 데미지, 범위, 지속시간은 컨텍스트의 기본 스탯 사용
    CachedContext = Context;
    float FinalDamage = Context.Damage;
    float FinalRadius = Context.Radius; // 혹은 Context.Range 등 스탯 이름에 맞춰 사용
    float FinalDuration = Context.Duration;

    // [중요] 타격 간격(HitCooldown) 결정 로직
    // 방법 A: 컨텍스트 Map에 "HitInterval"이라는 값이 있으면 쓰고, 없으면 0.5초 기본값
    float FinalHitCooldown = Context.GetValue(TEXT("HitInterval"), 0.5f);

    // 방법 B: (선택사항) 공격 속도(Speed) 스탯이 높으면 타격 간격이 짧아지게 하려면?
    // if (Context.Speed > 0.f) FinalHitCooldown = 1.0f / Context.Speed;

    // 3. 메인 초기화 함수 호출
    InitializeAOE(FinalDamage, FinalRadius, FinalHitCooldown, FinalDuration);

    // 4. 확장 스탯 적용 (Map에서 꺼내오기)
    // 넉백 (Knockback)
    float KForce = Context.KnockbackForce; // 혹은 Context.GetValue("Knockback")
    if (KForce > 0.f)
    {
        // 넉백 힘을 멤버 변수에 저장하거나 설정 (멤버 변수가 있다고 가정)
        // KnockbackForce = KForce; 
    }

    // 크기 보정 (Radius와 별개로 비주얼만 키우고 싶을 때)
    float VisualScale = Context.GetValue(TEXT("Scale"), 1.0f);
    if (VisualScale != 1.0f)
    {
        SetActorScale3D(FVector(VisualScale));
    }
}

void ADESimpleAOEBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    
    // 나 자신이나 주인은 무시
    if (!OtherActor || OtherActor == this || OtherActor == OwnerActor.Get()) return;
    // 3. [핵심] 몬스터의 "핵심 부위(Root)"가 아니면 무시! (팔, 다리, 메쉬 충돌 방지)
    if (OtherComp != OtherActor->GetRootComponent())
    {
        return;
    }
    //UE_LOG(LogTemp, Warning, TEXT("%s OnOverlapBegin"), *GetName());
    // 들어오자마자 쿨타임 체크 후 즉시 타격 (반응성 UP)
    if (CanHitTarget(OtherActor))
    {
        OnHitTarget(OtherActor);

        // 다음 타격 시간 기록
        double CurrentTime = GetWorld()->GetTimeSeconds();
        HitCooldownMap.Add(OtherActor, CurrentTime + HitCooldown);
    }
}

void ADESimpleAOEBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 현재 겹쳐있는 모든 액터 가져오기
    TArray<AActor*> OverlappingActors;
    Collision->GetOverlappingActors(OverlappingActors);

    const double CurrentTime = GetWorld()->GetTimeSeconds();

    for (AActor* Target : OverlappingActors)
    {
        if (!Target || Target == OwnerActor.Get()) continue;

        // 쿨타임 지났는지 확인
        if (CanHitTarget(Target))
        {
            OnHitTarget(Target);
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
    TryDealDamage(Target);
    //UE_LOG(LogTemp, Warning, TEXT("%s got hit by : %s"), *Target->GetName(), *GetName());
    // [확장 포인트]
    // 여기서 넉백(Knockback)이나 상태이상(ApplyStatus) 로직을 추가하면 됩니다.
    // 예: ApplyKnockback(Target);
}


bool ADESimpleAOEBase::TryDealDamage(AActor* Victim)
{
    if (!Victim) return false;
    // 1. 장판(AOE) 특유의 넉백 방향 계산
    // 장판 중심(또는 플레이어)에서 바깥으로 밀어냅니다.
    FVector KBDir = Victim->GetActorLocation() - GetActorLocation();

    // (선택 디테일) 장판 넉백은 보통 공중으로 안 뜨게 Z축을 무시합니다.
    KBDir.Z = 0.0f;
    KBDir.Normalize();

    // 2. 주문서(Request) 작성
    FDEDamageRequest Req;
    Req.Instigator = DamageInstigator.Get();
    Req.DamageCauser = this;
    
    Req.SourceObject = CachedContext.SourceSkill;
    Req.Victim = Victim;
    Req.BaseDamage = Damage;
    Req.CritChance = CritChance;
    Req.CritDamageMultiplier = CritDamageMultiplier;
    // 3. 순수한 계산기(Library)에 던지기
    FDEDamageResult Res = UDEGameplayLibrary::ApplyCombatDamage(Req);
   /* if (Res.FinalDamage > 0.0f && this->KnockbackForce > 0.0f)
    {
        if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Req.Victim))
            Monster->ApplyKnockback(KBDir, this->KnockbackForce);
    }*/

    return Res.FinalDamage > 0.0f;
}

void ADESimpleAOEBase::LifeSpanExpired()
{
    ReturnToPool();
}

void ADESimpleAOEBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 혹시라도 Destroy로 삭제될 때 타이머나 바인딩 정리
    Collision->OnComponentBeginOverlap.RemoveDynamic(this, &ADESimpleAOEBase::OnOverlapBegin);
    Super::EndPlay(EndPlayReason);
}

void ADESimpleAOEBase::ReturnToPool()
{
    // 수명 초기화 (SetLifeSpan(0) 안 하면 풀에 가서도 죽음)
    SetLifeSpan(0);
    HitCooldownMap.Reset();
    //UE_LOG(LogTemp, Warning, TEXT("%s Returned To Pool, hitcooldownmap cleared"), *GetName());
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (auto* Pool = World->GetSubsystem<UDEPoolSubsystem>())
            {
                Pool->ReturnActorToPool(this);
            }
        }
    }
}