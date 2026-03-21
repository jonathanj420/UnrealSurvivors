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

    if (GetOwner())
    {
        if (UDEDamageTextSubsystem* DmgSys = GetWorld()->GetSubsystem<UDEDamageTextSubsystem>())
        {
            FDamageVisualInfo DmgInfo;
            DmgInfo.WorldLocation = GetOwner()->GetActorLocation() + FVector(0, 0, 100);
            DmgInfo.Amount = HealAmount;
            DmgInfo.TextType = EDamageTextType::Heal;

            DmgSys->ShowDamage(DmgInfo);
        }
    }

    OnHPChanged.Broadcast(CurrentHP, MaxHP);
}

void UDEHealthComponent::ResetHealth(bool bHealToFull)
{
    // 1. 사망 플래그 초기화 (가장 중요!)
    bIsDead = false;
    //UE_LOG(LogTemp, Error, TEXT("HP Reset, bisdead NO FUCK"));
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
   // UE_LOG(LogTemp, Error, TEXT("Actually Died"));
    // 여기서는 Destroy / 애니메이션 처리 안 함
    // 소유 Actor(Character/Monster)가 책임지게 함 (AAA 스타일)
}

FDEDamageResult UDEHealthComponent::ProcessDamage(const FDEDamageRequest& Request)
{
    FDEDamageResult Result;

    // 1. 요청받은 정보들을 결과(Result)에 그대로 복사해서 '기억'시킴!
    Result.Victim = Request.Victim;
    Result.SourceObject = Request.SourceObject;
    Result.DamageType = Request.DamageType;

    // 2. 이미 죽었으면 무시
    if (bIsDead)
    {
        Result.bIsDead = true;
        return Result;
    }

    float CalculatedDamage = 0.0f;
    bool bCriticalSuccess = false;

    // =========================================================
    // ★ 3. Enum으로 완벽하게 분리된 데미지 파이프라인!
    // =========================================================
    switch (Request.DamageType)
    {
    case EDEDamageType::Execution:
    case EDEDamageType::InstantKill:
    {
        //  처형이나 즉사는 묻지도 따지지도 않고 남은 피통만큼 데미지!
        CalculatedDamage = CurrentHP;
        bCriticalSuccess = true; // 처형 연출을 위해 강제 크리티컬 취급
        break;
    }

    case EDEDamageType::Poison:
    case EDEDamageType::Bleed:
    {
        //  도트 데미지는 크리티컬이나 방어력을 무시하게 짤 수도 있음
        CalculatedDamage = Request.BaseDamage;
        bCriticalSuccess = false;
        break;
    }

    case EDEDamageType::Normal:
    default:
    {
        //  일반 타격 (치명타 주사위 굴리기)
        if (Request.CritChance > 0.0f)
        {
            bCriticalSuccess = FMath::RandRange(0.0f, 1.0f) < Request.CritChance;
        }
        CalculatedDamage = bCriticalSuccess ? (Request.BaseDamage * Request.CritDamageMultiplier) : Request.BaseDamage;

        // Result.FinalDamage -= MyDefense; // (확장) 나중에 방어력 깎는 로직
        break;
    }
    }

    Result.FinalDamage = CalculatedDamage;
    Result.bIsCritical = bCriticalSuccess;

    // 4. 실제 체력 차감 및 UI 처리 (여기서 Request.DamageCauser를 넘기기 때문에 쿨감 버그 해결!)
    if (Result.FinalDamage > 0.0f)
    {
        ApplyFinalDamage(Result.FinalDamage, Request.DamageCauser, bCriticalSuccess, Request.DamageType);
    }

    // 5. ApplyFinalDamage를 거치면서 죽었을 수 있으므로 갱신
    Result.bIsDead = bIsDead;
    if (Result.bIsDead)
    {
        UE_LOG(LogTemp, Warning, TEXT("So dead"), Result.FinalDamage);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Not So Ded"));
    }
    
    return Result;
}

void UDEHealthComponent::ApplyFinalDamage(float InDamage, AActor* InCauser, bool bInIsCritical, EDEDamageType InDamageType)
{
    if (bIsDead) return;

    CurrentHP = FMath::Clamp(CurrentHP - InDamage, 0.0f, MaxHP);

    // 체력바 갱신
    OnHPChanged.Broadcast(CurrentHP, MaxHP);

    // 데미지 텍스트 띄우기
    if (UWorld* World = GetWorld())
    {
        if (UDEDamageTextSubsystem* DmgSys = World->GetSubsystem<UDEDamageTextSubsystem>())
        {
            FDamageVisualInfo DmgInfo;
            DmgInfo.Amount = InDamage;
            if (GetOwner())
            {
                DmgInfo.WorldLocation = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, 100.f);
            }

            // =========================================================
            // ★ 텍스트 타입 분기 (처형 텍스트 처리!)
            // =========================================================
            if (InDamageType == EDEDamageType::Execution)
            {
                DmgInfo.TextType = EDamageTextType::Execution; // "Executed!" 출력
            }
            else if (bInIsCritical)
            {
                DmgInfo.TextType = EDamageTextType::Critical; // 크고 빨간 숫자
            }
            else
            {
                DmgInfo.TextType = EDamageTextType::Damage; // 일반 하얀 숫자
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


void UDEHealthComponent::InstantKill(AActor* Executioner, bool bShowDamage /* = false */)
{
    if (bIsDead) return;

    // 1. 일반 즉사 데미지 연출 (함정, 폭발 등)
    if (bShowDamage && GetOwner())
    {
        if (UDEDamageTextSubsystem* DmgSys = GetWorld()->GetSubsystem<UDEDamageTextSubsystem>())
        {
            FDamageVisualInfo DmgInfo;
            DmgInfo.WorldLocation = GetOwner()->GetActorLocation() + FVector(0, 0, 100);
            DmgInfo.Amount = MaxHP; // 최대 체력만큼의 데미지 숫자!
            DmgInfo.TextType = EDamageTextType::Critical; // 크고 빨간 숫자로 띄움

            DmgSys->ShowDamage(DmgInfo);
        }
    }

    // 2. 데이터 처리
    CurrentHP = 0.0f;
    OnHPChanged.Broadcast(CurrentHP, MaxHP);
    HandleDeath(Executioner);

    //// 1. 이미 죽은 놈은 두 번 죽이지 않음
    //if (bIsDead) return;

    //// 2. 쉴드, 방어력 계산 다 무시하고 체력을 강제로 증발시킴
    //CurrentHP = 0.0f;

    //// 3. UI 갱신 (체력바 즉시 0으로)
    //OnHPChanged.Broadcast(CurrentHP, MaxHP);

    //// 4. (연출) 서브시스템을 통한 처형 데미지 텍스트 띄우기
    //if (UWorld* World = GetWorld())
    //{
    //    if (UDEDamageTextSubsystem* DmgSys = World->GetSubsystem<UDEDamageTextSubsystem>())
    //    {
    //        FDamageVisualInfo DmgInfo;
    //        // 텍스트 연출을 위해 남은 체력이나 최대 체력만큼 띄워줌
    //        DmgInfo.Amount = MaxHP;
    //        DmgInfo.bIsCritical = true; // 처형이니까 크리티컬 이펙트 빌려 쓰기!

    //        if (GetOwner())
    //        {
    //            DmgInfo.WorldLocation = GetOwner()->GetActorLocation() + FVector(0, 0, 100);
    //        }
    //        DmgSys->ShowDamage(DmgInfo);
    //        UE_LOG(LogTemp, Warning, TEXT("EXECUTED ! ! !"));
    //    }
    //}

    //// 5. 진짜 사망 처리로 넘김
    //HandleDeath(Executioner);
}

