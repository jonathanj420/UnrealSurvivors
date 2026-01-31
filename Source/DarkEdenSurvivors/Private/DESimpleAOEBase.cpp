// Fill out your copyright notice in the Description page of Project Settings.


#include "DESimpleAOEBase.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DEPoolSubsystem.h"
#include "DESkillContext.h"

ADESimpleAOEBase::ADESimpleAOEBase()
{
    // [중요] 지속 데미지(Tick)를 줘야 하므로 Tick을 켜야 합니다.
    PrimaryActorTick.bCanEverTick = true;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;

    // [중요] 충돌을 켜야 감지가 됩니다. 'OverlapAllDynamic' 추천 (Pawn, Enemy 등 감지)
    Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    Collision->SetGenerateOverlapEvents(true); // 이벤트 발생 필수

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

    //Context에서 기본 스탯 추출
    const float FinalDamage = Context.Damage;
    const float FinalRadius = Context.Radius;
    const float FinalDuration = Context.Duration;

    //타격 간격 (확장 가능)
    const float FinalHitCooldown =
        Context.GetValue(TEXT("HitInterval"), 0.5f);

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
    // NiagaraComp->SetRelativeScale3D(FVector(EffectScale));

    // 맵 초기화 (재사용 시 필수)
    HitCooldownMap.Empty();

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
        return CurrentTime >= *NextHitTime;
    }

    // 기록이 없으면(처음 만남) 즉시 타격 가능
    return true;
}

void ADESimpleAOEBase::OnHitTarget(AActor* Target)
{
    DealDamage(Target);

    // [확장 포인트]
    // 여기서 넉백(Knockback)이나 상태이상(ApplyStatus) 로직을 추가하면 됩니다.
    // 예: ApplyKnockback(Target);
}

void ADESimpleAOEBase::DealDamage(AActor* Target)
{
    if (!Target) return;

    AActor* InstigatorActor = DamageInstigator.Get();

    UGameplayStatics::ApplyDamage(
        Target,
        Damage,
        InstigatorActor
        ? InstigatorActor->GetInstigatorController()
        : nullptr,
        InstigatorActor,
        UDamageType::StaticClass()
    );

    /*UGameplayStatics::ApplyDamage(
        Target,
        Damage,
        OwnerActor.IsValid() ? OwnerActor->GetInstigatorController() : nullptr,
        OwnerActor.Get(),
        UDamageType::StaticClass()
    );*/
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

    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GI = World->GetGameInstance())
        {
            if (auto* Pool = GI->GetSubsystem<UDEPoolSubsystem>())
            {
                Pool->ReturnActorToPool(this);
            }
        }
    }
}