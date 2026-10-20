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
void UDEHealthComponent::HandleDeath(AActor* Killer)
{
    if (bIsDead) return;

    bIsDead = true;
    CurrentHP = 0.f;

    OnHPChanged.Broadcast(CurrentHP, MaxHP);
    // OnDeath는 소유 액터가 사망 처리를 끝낸 뒤 broadcast한다
    OnDying.Broadcast(Killer);
    // 여기서는 Destroy / 애니메이션 처리 안 함
    // 소유 Actor(Character/Monster)가 책임지게 함 (AAA 스타일)
}

FDEDamageResult UDEHealthComponent::ProcessDamage(const FDEDamageRequest& Request)
{
    FDEDamageResult Result;

    // 1. 요청받은 정보들을 결과(Result)에 그대로 복사해서 '기억'시킴!
    /*Result.Victim = Request.Victim;
    Result.SourceObject = Request.SourceObject;
    Result.DamageType = Request.DamageType;
    Result.bCanTriggerOnHit = Request.bCanTriggerOnHit;*/

    // 2. 이미 죽었으면 무시
    if (bIsDead)
    {
        Result.bIsDead = true;
        return Result;
    }

    float CalculatedDamage = 0.0f;
    bool bCriticalSuccess = false;

    static const FGameplayTag Tag_Execution = FGameplayTag::RequestGameplayTag(FName("Damage.Mechanic.Execution"));
    static const FGameplayTag Tag_InstantKill = FGameplayTag::RequestGameplayTag(FName("Damage.Mechanic.InstantKill"));
    static const FGameplayTag Tag_DoT = FGameplayTag::RequestGameplayTag(FName("Damage.Mechanic.DoT"));

    // 가방 안에 처형이나 즉사 태그가 들어있다면?
    if (Request.DamageTags.HasTag(Tag_Execution) || Request.DamageTags.HasTag(Tag_InstantKill))
    {
        CalculatedDamage = CurrentHP;
        bCriticalSuccess = true;
    }
    // 가방 안에 도트(DoT) 태그가 들어있다면?
    else if (Request.DamageTags.HasTag(Tag_DoT))
    {
        CalculatedDamage = Request.BaseDamage;
        bCriticalSuccess = false; // 도트는 크리티컬 안 터짐
    }
    // 그 외 일반 타격 (DirectHit 등)
    else
    {
        if (Request.CritChance > 0.0f)
        {
            bCriticalSuccess = FMath::RandRange(0.0f, 1.0f) < Request.CritChance;
        }
        CalculatedDamage = bCriticalSuccess ? (Request.BaseDamage * Request.CritDamageMultiplier) : Request.BaseDamage;
    }
    Result.FinalDamage = CalculatedDamage;
    Result.bIsCritical = bCriticalSuccess;

    // 4. 실제 체력 차감 및 UI 처리. 킬은 스킬 액터(DamageCauser)가 아니라 시전자(Instigator)에게 돌아간다
    if (Result.FinalDamage > 0.0f)
    {
        ApplyFinalDamage(Result.FinalDamage, Request.Instigator, bCriticalSuccess, Request.DamageTags);
    }

    // 5. ApplyFinalDamage를 거치면서 죽었을 수 있으므로 갱신
    Result.bIsDead = bIsDead;

    return Result;
}

void UDEHealthComponent::ApplyFinalDamage(float InDamage, AActor* InInstigator, bool bInIsCritical, const FGameplayTagContainer& InDamageTags)
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
            // ★ 텍스트 타입 분기 (태그 가방 검사!)
            // =========================================================
            // 캐싱해 둔 태그들
            static const FGameplayTag Tag_Execution = FGameplayTag::RequestGameplayTag(FName("Damage.Mechanic.Execution"));
            static const FGameplayTag Tag_DoT = FGameplayTag::RequestGameplayTag(FName("Damage.Mechanic.DoT"));
            static const FGameplayTag Tag_Poison = FGameplayTag::RequestGameplayTag(FName("Damage.Affinity.Poison"));

            // 1. 처형인가?
            if (InDamageTags.HasTag(Tag_Execution))
            {
                DmgInfo.TextType = EDamageTextType::Execution; // "Executed!"
            }
            // 2. 도트 딜인가? (기존 상태이상 텍스트 재활용)
            else if (InDamageTags.HasTag(Tag_DoT))
            {
                // (확장) 만약 독 속성이면 텍스트 색을 바꾸고 싶다면?
                // 서브시스템 쪽에서 DmgInfo.TextType 을 세분화해서 녹색 틱뎀 띄우기도 가능합니다.
                DmgInfo.TextType = EDamageTextType::StatusEffect;
            }
            // 3. 크리티컬인가? (일반 타격 중 크리 터진 놈)
            else if (bInIsCritical)
            {
                DmgInfo.TextType = EDamageTextType::Critical; // 크고 빨간 숫자
            }
            // 4. 전부 아니면 일반 타격!
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
        HandleDeath(InInstigator);
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

}

