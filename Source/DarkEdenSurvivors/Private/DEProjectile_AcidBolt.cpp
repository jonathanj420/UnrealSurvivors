// Fill out your copyright notice in the Description page of Project Settings.


#include "DEProjectile_AcidBolt.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DEMonsterBase.h"
#include "Engine/OverlapResult.h"
#include "DEGameplayLibrary.h"
#include "NiagaraComponent.h"
#include "DEAutoSkillBase.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ADEProjectile_AcidBolt::ADEProjectile_AcidBolt()
{
    CollisionComponent->InitSphereRadius(30.0f);
    ConstructorHelpers::FObjectFinder<UStaticMesh>SM_ABOLT(TEXT("/Game/DarkEden/Data/Skill/AcidBolt/AcidBolt.AcidBolt"));
    if (SM_ABOLT.Succeeded())
    {
        Mesh->SetStaticMesh(SM_ABOLT.Object);
    }
    Mesh->SetRelativeScale3D(FVector(4.0f, 4.0f, 4.0f));
    Mesh->SetRelativeLocationAndRotation(FVector(190.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
    MovementComponent->InitialSpeed = 1200.f;
    MovementComponent->MaxSpeed = 1200.f;

    static ConstructorHelpers::FObjectFinder<USoundBase> SoundObj(
        TEXT("/Game/DarkEden/Data/Sound/SkillSoundEffect/Custom/AcidBolt2.AcidBolt2")
    );

    if (SoundObj.Succeeded())
    {
        ExplosionSound = SoundObj.Object;
    }
    
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExplosionFX(
        TEXT("/Game/DarkEden/Data/Niagara/NS_AcidBoltExplode.NS_AcidBoltExplode")
    );

    if (ExplosionFX.Succeeded())
    {
        ExplosionEffect = ExplosionFX.Object;
    }


    //Combat
    Damage = 1.0f;
    Penetration = 10;
    KnockbackForce = 1600.0f;
    LifeTime = 3.0f;
    Speed = 1200.0f;
    Size = 30.0f;
    CritChance = 0.5f;
    bCanCrit = false;
    bIsCrit = false;


}

// Called when the game starts or when spawned
void ADEProjectile_AcidBolt::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void ADEProjectile_AcidBolt::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);


}



void ADEProjectile_AcidBolt::InitializeProjectile(float InDamage, float InSpeed, int32 InPenetration, const FVector& Direction)
{
    Super::InitializeProjectile(InDamage, InSpeed, InPenetration, Direction);

}

void ADEProjectile_AcidBolt::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{

    if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(OtherActor))
    {
        UGameplayStatics::ApplyDamage(Monster, Damage, GetInstigatorController(), this, UDamageType::StaticClass());
        //UE_LOG(LogTemp, Warning, TEXT("CALLED ONOVERLAP"));
        Explode();
    }
}

void ADEProjectile_AcidBolt::Explode()
{
    //UE_LOG(LogTemp, Warning, TEXT("CALLED EXPLODE"));
    UWorld* World = GetWorld();
    if (!World) return;

    FVector Center = GetActorLocation();

    // 1. [시각화] 폭발 범위 (초록색)
    //DrawDebugSphere(World, Center, EffectRadius, 16, FColor::Green, false, 1.0f, 0, 1.0f);

    //// 2. [검출] 범위 내 몬스터 찾기
    //TArray<FOverlapResult> OverlapResults;
    //FCollisionQueryParams QueryParams;
    //QueryParams.AddIgnoredActor(this); // 나 자신은 무시
    //QueryParams.AddIgnoredActor(GetOwner()); // 플레이어 무시

    //// 몬스터 채널(ECC_GameTraceChannel1 등)로만 검사 -> 성능 최적화
    //bool bHitAny = World->OverlapMultiByChannel(
    //    OverlapResults,
    //    Center,
    //    FQuat::Identity,
    //    ECC_GameTraceChannel5, // ★ 프로젝트 설정에 맞는 몬스터 채널 사용 필수!
    //    FCollisionShape::MakeSphere(EffectRadius),
    //    QueryParams
    //);
    TArray<FOverlapResult> OverlapResults;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(GetOwner());

    // ★ 핵심 변경: ObjectType으로 검색하기 위한 설정
    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel5); // 몬스터 채널(5번)만 콕 집어서 검색

    // 함수 변경: OverlapMultiByChannel -> OverlapMultiByObjectType
    bool bHitAny = World->OverlapMultiByObjectType(
        OverlapResults,
        Center,
        FQuat::Identity,
        ObjectQueryParams, // ★ 채널 대신 오브젝트 쿼리 파라미터가 들어감
        FCollisionShape::MakeSphere(EffectRadius),
        QueryParams
    );

    if (bHitAny)
    {
        // 중복 타격 방지용 (한 몬스터에 콜라이더가 여러 개일 수 있음)
        //TSet<AActor*> HitActors;
        //UE_LOG(LogTemp, Warning, TEXT("Target Hit"));
        for (const FOverlapResult& Result : OverlapResults)
        {
            AActor* TargetActor = Result.GetActor();
            if (TargetActor && !HitActors.Contains(TargetActor))
            {
                HitActors.Add(TargetActor);
                // 4. [시각화] 피격된 몬스터 위치에 점 찍기 (빨간색)
                //DrawDebugSphere(World, TargetActor->GetActorLocation(), 30.0f, 8, FColor::Red, false, 1.0f, 0, 1.0f);
                // 3. [처리] 데미지 주기 (1:1 타격으로 변경)
                //UGameplayStatics::ApplyDamage(
                //    TargetActor,
                //    Damage, // 거리 비례 감소 없이 100% 데미지
                //    GetInstigatorController(),
                //    this,
                //    UDamageType::StaticClass()
                //);

                FDEDamageRequest Req;
                Req.Instigator = GetInstigator();
                Req.DamageCauser = this;
                Req.SourceObject = CachedContext.SourceSkill;
                Req.Victim = TargetActor;
                Req.BaseDamage = Damage;

                // 1. [데미지 선 적용] 라이브러리 호출
                FDEDamageResult Res = UDEGameplayLibrary::ApplyCombatDamage(Req);



               // UE_LOG(LogTemp, Warning, TEXT("Hit %d Targets / acidbolt"), HitActors.Num());

                // (옵션) 여기서 넉백(ApplyKnockback)도 직접 호출 가능!
                /*
                ADEMonsterBase* Monster = Cast<ADEMonsterBase>(TargetActor);
                if(Monster) { ... }
                */
            }
        }
        
    }
    else
    {
        //UE_LOG(LogTemp, Warning, TEXT("Exploded but no targets"));
    }
    if (ExplosionEffect)
    {
        UNiagaraComponent* SpawnedEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            World,
            ExplosionEffect,
            Center,
            GetActorRotation()
        );

        if (SpawnedEffect)
        {
            // 물리 타격 판정과 완전히 동일한 'EffectRadius'를 나이아가라로 쏴줌!
            SpawnedEffect->SetFloatParameter(FName("SkillRadius"), EffectRadius);
        }
    }
    // ---- 2) 소리 간헐 재생 ----
    static double LastExplosionSoundTime = 0.0;
    const double CurrentTime = FPlatformTime::Seconds();

    // 최소 간격 0.05초
    const double SoundCooldown = 1.0f;

    if (ExplosionSound && (CurrentTime - LastExplosionSoundTime >= SoundCooldown))
    {
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
        LastExplosionSoundTime = CurrentTime;
    }
   // UE_LOG(LogTemp, Warning, TEXT("ACIDBOLT RETURNING TO POOL"));
    ReturnToPool();
}

void ADEProjectile_AcidBolt::OnLifeTimeExpired()
{
    Explode();
}
