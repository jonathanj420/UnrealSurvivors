// Fill out your copyright notice in the Description page of Project Settings.


#include "DEHealthComponent.h"
#include "DEDamageTextSubsystem.h" // 서브시스템 불러오기
#include "DEDamageTypes.h"         // 데이터 구조체 불러오기

UDEHealthComponent::UDEHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UDEHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    // 안전장치: 시작 시 체력 정합성 보장
    CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);
}

//void UDEHealthComponent::ApplyDamage(float DamageAmount, AActor* DamageCauser)
//{
//    if (bIsDead) return;
//    if (DamageAmount <= 0.f) return;
//
//    CurrentHP -= DamageAmount;
//    CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);
//
//
//    // 체력 변경 이벤트
//    OnHPChanged.Broadcast(CurrentHP, MaxHP);
//
//    // [AAA 스타일 호출] 서브시스템이 있는지 확인하고 호출
//    if (UWorld* World = GetWorld())
//    {
//        // 서브시스템 가져오기 (싱글톤처럼 작동)
//        if (UDEDamageTextSubsystem* DmgSys = World->GetSubsystem<UDEDamageTextSubsystem>())
//        {
//            // 1. 택배 상자(구조체) 만들기
//            FDamageVisualInfo DmgInfo;
//            DmgInfo.Amount = DamageAmount; // 실제 입힌 데미지
//
//            // 2. 위치 설정 (몬스터 머리 위)
//            // GetOwner()는 이 컴포넌트를 가진 몬스터 액터
//            if (GetOwner())
//            {
//                DmgInfo.WorldLocation = GetOwner()->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
//                // Z축 +100.0f 해서 머리 쯤에 띄우기
//            }
//
//            // 3. 크리티컬 여부 (만약 로직이 있다면)
//            // DmgInfo.bIsCritical = bIsCritical; 
//
//            // 4. 발사!
//            DmgSys->ShowDamage(DmgInfo);
//        }
//    }
//
//    if (CurrentHP <= 0.f)
//    {
//        HandleDeath(DamageCauser);
//    }
//
//}

void UDEHealthComponent::Heal(float HealAmount)
{
    if (bIsDead) return;
    if (HealAmount <= 0.f) return;

    CurrentHP += HealAmount;
    CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);

    OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

void UDEHealthComponent::ResetHealth(bool bHealToFull)
{
    // 1. 사망 플래그 초기화 (가장 중요!)
    bIsDead = false;

    //// 2. 무적 상태 및 타이머 초기화
    //bIsInvincible = false;
    //if (GetWorld())
    //{
    //    GetWorld()->GetTimerManager().ClearTimer(InvincibleTimerHandle);
    //}

    // 3. 체력 복구
    if (bHealToFull)
    {
        CurrentHP = MaxHP;
    }

    // 4. 리셋된 상태를 UI 등에 즉시 알림
    OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

void UDEHealthComponent::SetMaxHP(float NewMaxHP, bool bHealToFull)
{
    MaxHP = FMath::Max(NewMaxHP, 1.f);

    if (bHealToFull)
    {
        CurrentHP = MaxHP;
    }
    else
    {
        CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);
    }

    OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

void UDEHealthComponent::IncreaseMaxHP(float Amount, bool bHealToFull)
{
    SetMaxHP(MaxHP + Amount, bHealToFull);
}

void UDEHealthComponent::SetCurrentHP(float NewHP)
{
    // 1. 값 보정 (0 ~ MaxHP 사이로 강제 고정)
    float ClampedHP = FMath::Clamp(NewHP, 0.0f, MaxHP);

    // 2. 값이 변하지 않았다면 굳이 이벤트 쏠 필요 없음 (최적화)
    if (FMath::IsNearlyEqual(CurrentHP, ClampedHP))
    {
        return;
    }

    // 3. 값 적용
    CurrentHP = ClampedHP;

    // 4. UI 갱신 알림 (중요!)
    // 이거 안 하면 체력은 100인데 체력바는 0으로 보이는 버그 생김
    if (OnHPChanged.IsBound())
    {
        OnHPChanged.Broadcast(CurrentHP, MaxHP);
    }

    // 5. 사망 처리 체크
    // 만약 치트키로 HP를 0으로 설정했다면 죽어야 함
    if (CurrentHP <= 0.0f && !bIsDead)
    {
        HandleDeath(nullptr); // 강제 설정이라 가해자(DamageCauser)는 없음
    }
    // 6. 부활 처리 (옵션)
    // 만약 죽은 상태(bIsDead)인데 HP를 양수로 설정했다면? -> 부활!
    else if (bIsDead && CurrentHP > 0.0f)
    {
        bIsDead = false;
        // 필요하다면 OnResurrect 같은 이벤트 추가 브로드캐스트
    }
}
void UDEHealthComponent::HandleDeath(AActor* DamageCauser)
{
    if (bIsDead) return;

    bIsDead = true;
    CurrentHP = 0.f;

    OnHPChanged.Broadcast(CurrentHP, MaxHP);
    OnDeath.Broadcast();

    // 여기서는 Destroy / 애니메이션 처리 안 함
    // 소유 Actor(Character/Monster)가 책임지게 함 (AAA 스타일)
}

FDEDamageResult UDEHealthComponent::ProcessDamage(const FDEDamageRequest& Request)
{
    FDEDamageResult Result;

    // 1. 이미 죽었거나 무적이면 데미지 0 처리
    if (bIsDead /* || bIsInvincible */)
    {
        return Result; // FinalDamage = 0, bIsDead = true/false
    }

    // 2. 치명타 주사위 굴리기 (RNG)
    // (나중에 '치명타 저항' 스탯이 생기면 여기서 Request.CritChance를 깎음)
    bool bCriticalSuccess = false;
    if (Request.CritChance > 0.0f)
    {
        bCriticalSuccess = FMath::RandRange(0.0f, 1.0f) < Request.CritChance;
    }

    // 3. 데미지 계산
    if (bCriticalSuccess)
    {
        Result.FinalDamage = Request.BaseDamage * Request.CritDamageMultiplier;
        Result.bIsCritical = true;
    }
    else
    {
        Result.FinalDamage = Request.BaseDamage;
        Result.bIsCritical = false;
    }

    // 4. (확장성) 방어력(Defense) 적용 로직이 들어갈 자리
    // Result.FinalDamage = FMath::Max(Result.FinalDamage - MyDefense, 1.0f);

    // 5. 실제 체력 차감 및 UI 처리 (내부 함수 호출)
    if (Result.FinalDamage > 0.0f)
    {
        ApplyFinalDamage(Result.FinalDamage, Request.DamageCauser, Result.bIsCritical);
    }

    // 6. 결과 갱신 (죽었는지 확인)
    Result.bIsDead = bIsDead;

    return Result;
}

void UDEHealthComponent::ApplyFinalDamage(float InDamage, AActor* InCauser, bool bInIsCritical)
{
    if (bIsDead) return;

    CurrentHP = FMath::Clamp(CurrentHP - InDamage, 0.0f, MaxHP);

    // UI 알림
    OnHPChanged.Broadcast(CurrentHP, MaxHP);

    // 데미지 텍스트 서브시스템 호출
    if (UWorld* World = GetWorld())
    {
        if (UDEDamageTextSubsystem* DmgSys = World->GetSubsystem<UDEDamageTextSubsystem>())
        {
            FDamageVisualInfo DmgInfo;
            DmgInfo.Amount = InDamage;
            if (GetOwner())
            {
                DmgInfo.WorldLocation = GetOwner()->GetActorLocation() + FVector(0, 0, 100);
            }
            DmgInfo.bIsCritical = bInIsCritical; // ★ 여기서 빨간색 여부 결정
            if (bInIsCritical)
            {
                UE_LOG(LogTemp, Warning, TEXT("Kritz!!"));
            }
            DmgSys->ShowDamage(DmgInfo);
        }
    }

    // 사망 처리
    if (CurrentHP <= 0.0f)
    {
        HandleDeath(InCauser);
    }
}
