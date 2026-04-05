// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffectComponent.h"
#include "DEStatusEffectBase.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UDEStatusEffectComponent::UDEStatusEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false; // 걸린 게 없을 땐 Tick 끄기!

	// ...
}


// Called when the game starts
void UDEStatusEffectComponent::BeginPlay()
{
    Super::BeginPlay();

    // 컴포넌트를 소유한 액터(플레이어 또는 몬스터) 캐싱
    OwnerActor = GetOwner();
	
}


// Called every frame
void UDEStatusEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerActor.IsValid()) return;

	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		FActiveStatusEffect& EffectData = ActiveEffects[i];

		if (!EffectData.EffectDef)
		{
			ActiveEffects.RemoveAtSwap(i);
			continue;
		}

		EffectData.EffectDef->Tick(OwnerActor.Get(), EffectData, DeltaTime);

		if (EffectData.Duration > 0.f && EffectData.ElapsedTime >= EffectData.Duration)
		{
			EffectData.EffectDef->OnRemove(OwnerActor.Get(), EffectData);
			ActiveEffects.RemoveAtSwap(i);
		}
	}

	if (ActiveEffects.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
}

void UDEStatusEffectComponent::AddEffect(TSubclassOf<UDEStatusEffectBase> EffectClass, AActor* Instigator, float Duration, float Power, float Interval)
{
	if (!EffectClass || !OwnerActor.IsValid()) return;

	const UDEStatusEffectBase* CDO = GetDefault<UDEStatusEffectBase>(EffectClass);
	if (!CDO) return;

	// =========================================================
	// ★ 1. 면역 체크 (태그 가방 교집합 검사!)
	// 내 면역 가방(ImmuneTags)과 들어오는 디버프의 정체성(StatusTags)이 하나라도 겹치면 무시!
	// =========================================================
	if (ImmuneTags.HasAny(CDO->StatusTags)) return;

	// =========================================================
	// ★ 2. 중첩(Stack) 정책 처리
	// 이제 Enum 대신, "이미 내 몸에 똑같은 스킬(Class)이 묻어있는가?"로 검사합니다.
	// =========================================================
	FActiveStatusEffect* Existing = nullptr;
	for (FActiveStatusEffect& Effect : ActiveEffects)
	{
		if (Effect.EffectDef && Effect.EffectDef->GetClass() == EffectClass)
		{
			Existing = &Effect;
			break;
		}
	}

	if (Existing)
	{
		switch (CDO->StackPolicy)
		{
		case EStackPolicy::Ignore:
			return;

		case EStackPolicy::Refresh:
			Existing->ElapsedTime = 0.f;
			Existing->Power = FMath::Max(Existing->Power, Power);
			return;

		case EStackPolicy::Stack:
			// (참고: CDO에 MaxStacks 변수가 있다고 가정합니다)
			if (Existing->CurrentStacks < CDO->MaxStacks)
			{
				Existing->CurrentStacks++;
				CDO->OnStacked(OwnerActor.Get(), *Existing, Existing->CurrentStacks);
			}
			return;

		case EStackPolicy::Replace:
			CDO->OnRemove(OwnerActor.Get(), *Existing);

			Existing->Instigator = Instigator;
			Existing->Duration = Duration;
			Existing->Power = Power;
			Existing->Interval = Interval;
			Existing->ElapsedTime = 0.f;
			Existing->TickTimer = 0.f;
			Existing->CurrentStacks = 1;

			CDO->OnApply(OwnerActor.Get(), *Existing);
			return;
		}
	}

	// 3. 필터를 모두 통과했다면 새 구조체 적용
	FActiveStatusEffect NewEffect;
	NewEffect.EffectDef = CDO;
	NewEffect.Instigator = Instigator;
	NewEffect.Duration = Duration;
	NewEffect.Power = Power;
	NewEffect.Interval = Interval;
	NewEffect.ElapsedTime = 0.f;
	NewEffect.TickTimer = 0.f;
	NewEffect.CurrentStacks = 1;

	CDO->OnApply(OwnerActor.Get(), NewEffect);
	ActiveEffects.Add(NewEffect);

	SetComponentTickEnabled(true);
}

void UDEStatusEffectComponent::RemoveEffectsByTag(FGameplayTag Tag)
{
	// 태그가 비어있으면 무시
	if (!Tag.IsValid() || !OwnerActor.IsValid()) return;

	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		FActiveStatusEffect& EffectData = ActiveEffects[i];

		// 이 상태이상 거푸집의 '정체성 태그 가방' 안에 해당 태그가 들어있다면? 삭제!
		if (EffectData.EffectDef && EffectData.EffectDef->StatusTags.HasTag(Tag))
		{
			EffectData.EffectDef->OnRemove(OwnerActor.Get(), EffectData);
			ActiveEffects.RemoveAtSwap(i);
		}
	}

	if (ActiveEffects.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
}

void UDEStatusEffectComponent::RemoveAllEffects()
{
	if (!OwnerActor.IsValid()) return;

	// 캐릭터 사망, 혹은 보스의 페이즈 전환(완전 정화) 패턴 시 호출됩니다.
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		FActiveStatusEffect& EffectData = ActiveEffects[i];
		if (EffectData.EffectDef)
		{
			// 싹 다 원상복구 로직 실행
			EffectData.EffectDef->OnRemove(OwnerActor.Get(), EffectData);
		}
	}

	// 배열 메모리를 날리지 않고 크기만 0으로 만듭니다 (TArray의 Empty 재할당 방지 최적화)
	ActiveEffects.Reset();

	// 컴포넌트 틱 끄기
	SetComponentTickEnabled(false);
}

bool UDEStatusEffectComponent::HasEffectWithTag(FGameplayTag Tag) const
{
	if (!Tag.IsValid()) return false;

	for (const FActiveStatusEffect& EffectData : ActiveEffects)
	{
		// 태그 가방 검사!
		if (EffectData.EffectDef && EffectData.EffectDef->StatusTags.HasTag(Tag))
		{
			return true;
		}
	}

	return false;
}

void UDEStatusEffectComponent::ProcessIncomingDamageModifiers(FDEDamageRequest& InOutRequest)
{
	for (const FActiveStatusEffect& EffectData : ActiveEffects) // (배열 이름은 개발자님 코드에 맞게!)
	{
		if (EffectData.EffectDef)
		{
			// 각 상태이상 클래스(부식, 방어력 감소 등)의 훅을 차례대로 실행!
			EffectData.EffectDef->ModifyIncomingDamage(EffectData, InOutRequest);
		}
	}
}

FActiveStatusEffect* UDEStatusEffectComponent::GetEffectByTag(FGameplayTag Tag)
{
	for (FActiveStatusEffect& Effect : ActiveEffects)
	{
		// 이 상태이상 거푸집의 '정체성 태그 가방' 안에 해당 태그가 들어있는지 검사!
		if (Effect.EffectDef && Effect.EffectDef->StatusTags.HasTag(Tag))
		{
			return &Effect;
		}
	}
	return nullptr;
}

