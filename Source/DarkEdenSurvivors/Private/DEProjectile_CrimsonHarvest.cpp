// Fill out your copyright notice in the Description page of Project Settings.


#include "DEProjectile_CrimsonHarvest.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DEHealthComponent.h"
ADEProjectile_CrimsonHarvest::ADEProjectile_CrimsonHarvest()
{
    // 생성자 초기화
}

void ADEProjectile_CrimsonHarvest::ResetState()
{
    Super::ResetState();
    CurrentPhase = EBatPhase::Hunting; // 풀에서 꺼낼 때 다시 사냥 모드로!
    bHasEverDealt = false;

}

// 1. [핵심] 수명이 다했을 때 소멸하지 않고 페이즈 전환!
void ADEProjectile_CrimsonHarvest::OnLifeTimeExpired()
{
    if (CurrentPhase == EBatPhase::Hunting)
    {
        CurrentPhase = EBatPhase::Returning;

        // 투사체 수명을 임의로 늘려주거나 (혹은 틱에서 Returning일 때는 LifeTime 체크를 무시하도록 처리)

        // 돌아올 때 벽에 튕기는 것을 방지하려면 충돌 설정을 겹침(Overlap) 전용으로 바꾸는 것도 좋습니다.
        // CollisionComponent->SetCollisionProfileName(TEXT("OverlapAll")); 
    }
}

// 2. 이동 로직: 상태에 따라 다르게 움직임
void ADEProjectile_CrimsonHarvest::UpdateMovement(float DeltaTime)
{
    if (CurrentPhase == EBatPhase::Hunting)
    {
        // 튕겨다니는 로직은 부모(SanguineBat 또는 SimpleProjectileBase)의 것을 그대로 사용
        Super::UpdateMovement(DeltaTime);
        return;
    }
    else if (CurrentPhase == EBatPhase::Returning && CachedContext.Instigator)
    {
        // [호밍 로직] 플레이어(Instigator)를 향해 부드럽게 방향을 틉니다.
        FVector TargetLoc = CachedContext.Instigator->GetActorLocation();
        FVector CurrentLoc = GetActorLocation();
        FVector ToTarget = (TargetLoc - CurrentLoc).GetSafeNormal();

        // 뱀서류 특유의 매끄러운 유도탄 궤적 만들기 (FMath::VInterpTo 사용)
        ShootDirection = FMath::VInterpTo(ShootDirection, ToTarget, DeltaTime, HomingTurnSpeed).GetSafeNormal();

        // 속도 갱신
        if (MovementComponent)
        {
            MovementComponent->Velocity = ShootDirection * ReturnSpeed;
        }

        // 거리 체크 (부메랑처럼)
        float DistSq = FVector::DistSquared(CurrentLoc, TargetLoc);
        if (DistSq <= 10000.0f) // 플레이어 반경 100 이내 도달 시
        {
            // 여기서 플레이어 체력 회복 로직(Heal Effect) 호출!
            if (UDEHealthComponent* Health = CachedContext.Instigator->FindComponentByClass<UDEHealthComponent>())
            {
                if (bHasEverDealt)
                {
                    Health->Heal(HealAmount);
                }
                
            }

            ReturnToPool(); // 회복 후 소멸
        }
    }
}

// 3. 충돌 로직: 상태에 따라 다르게 처리
void ADEProjectile_CrimsonHarvest::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (CurrentPhase == EBatPhase::Hunting)
    {
        // 사냥 중일 때는 기존 SanguineBat의 튕기는 로직 실행
        Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
        bHasEverDealt = true;
    }
    else if (CurrentPhase == EBatPhase::Returning)
    {
        // 귀환 중일 때 적을 통과하며 데미지를 줄지, 무시할지 결정
        // 만약 관통하며 돌아오게 하고 싶다면 여기서 TryDealDamage(OtherActor); 호출 후 튕기기(Reflection) 로직은 생략!
    }
}