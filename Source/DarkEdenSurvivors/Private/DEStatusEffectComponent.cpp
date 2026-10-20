// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffectComponent.h"
#include "DarkEdenSurvivors.h"
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

	AActor* Owner = OwnerActor.Get();
	if (!Owner) return;

	// Tick/OnRemove 안의 데미지가 OnHit/OnKill을 거쳐 이 배열에 AddEffect나 RemoveAllEffects를 부를 수 있다.
	// 그래서 콜백 뒤에는 인덱스로 다시 잡고, 만료된 효과는 배열에서 먼저 뺀 다음 OnRemove를 부른다
	TArray<FActiveStatusEffect, TInlineAllocator<4>> Expired;

	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		if (!ActiveEffects.IsValidIndex(i)) continue;

		const UDEStatusEffectBase* EffectDef = ActiveEffects[i].EffectDef;
		if (!EffectDef)
		{
			ActiveEffects.RemoveAtSwap(i);
			continue;
		}

		EffectDef->Tick(Owner, ActiveEffects[i], DeltaTime);

		if (!ActiveEffects.IsValidIndex(i)) continue;

		const FActiveStatusEffect& EffectData = ActiveEffects[i];
		if (EffectData.Duration > 0.f && EffectData.ElapsedTime >= EffectData.Duration)
		{
			Expired.Add(EffectData);
			ActiveEffects.RemoveAtSwap(i);
		}
	}

	for (FActiveStatusEffect& EffectData : Expired)
	{
		EffectData.EffectDef->OnRemove(Owner, EffectData);
	}

	if (ActiveEffects.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
}

namespace
{
	// 남은 시간과 새 지속시간 중 긴 쪽으로. 0 이하는 무한
	void RefreshDuration(FActiveStatusEffect& Effect, float NewDuration)
	{
		const float Remaining = Effect.Duration - Effect.ElapsedTime;
		Effect.Duration = (Effect.Duration <= 0.f || NewDuration < 0.f) ? -1.f : FMath::Max(Remaining, NewDuration);
		Effect.ElapsedTime = 0.f;
	}
}

void UDEStatusEffectComponent::AddEffect(TSubclassOf<UDEStatusEffectBase> EffectClass, AActor* Instigator, float Duration, float Power, float Interval, float Radius)
{
	//UE_LOG(LogTemp, Warning, TEXT("Try To Add Status Effect : %s"),*EffectClass->GetName());
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
	int32 ExistingIndex = INDEX_NONE;
	if (CDO->StackPolicy != EStackPolicy::Independent)
	{
		ExistingIndex = ActiveEffects.IndexOfByPredicate([EffectClass](const FActiveStatusEffect& Effect)
		{
			return Effect.EffectDef && Effect.EffectDef->GetClass() == EffectClass;
		});
	}

	if (ExistingIndex != INDEX_NONE)
	{
		FActiveStatusEffect& Existing = ActiveEffects[ExistingIndex];

		// 재적용(Refresh, Stack)은 Power와 범위를 둘 중 큰 값으로. 스킬 레벨이 올라 더 센 효과가 들어오면 이미 걸린 효과도 따라 오른다
		switch (CDO->StackPolicy)
		{
		case EStackPolicy::Ignore:
			return;

		case EStackPolicy::Refresh:
			RefreshDuration(Existing, Duration);
			Existing.Power = FMath::Max(Existing.Power, Power);
			Existing.Radius = FMath::Max(Existing.Radius, Radius);
			return;

		case EStackPolicy::Stack:
			Existing.Power = FMath::Max(Existing.Power, Power);
			Existing.Radius = FMath::Max(Existing.Radius, Radius);
			// 최대 스택이어도 지속시간은 갱신한다
			if (CDO->bRefreshDurationOnStack)
			{
				RefreshDuration(Existing, Duration);
			}
			// MaxStacks가 0 이하면 무한 중첩
			if (CDO->MaxStacks <= 0 || Existing.CurrentStacks < CDO->MaxStacks)
			{
				Existing.CurrentStacks++;
				CDO->OnStacked(OwnerActor.Get(), Existing, Existing.CurrentStacks);
			}
			return;

		case EStackPolicy::Replace:
		{
			// 옛 효과를 배열에서 먼저 빼고 OnRemove. 그 안의 데미지가 AddEffect를 불러 배열이 재할당돼도 안전하다. 새 효과는 아래에서 추가
			FActiveStatusEffect Old = Existing;
			ActiveEffects.RemoveAtSwap(ExistingIndex);
			CDO->OnRemove(OwnerActor.Get(), Old);
			if (!OwnerActor.IsValid()) return;
			break;
		}

		default:
			break;
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
	NewEffect.Radius = Radius;

	if (Duration == 0.f)
	{
		// 1. 적용 즉시
		CDO->OnApply(OwnerActor.Get(), NewEffect);

		// 2. 해제 로직(폭발, 즉발 힐 등) 바로 실행!
		CDO->OnRemove(OwnerActor.Get(), NewEffect);

		// 3. 배열에 넣지 않고 여기서 깔끔하게 퇴근!
		return;
	}

	CDO->OnApply(OwnerActor.Get(), NewEffect);
	ActiveEffects.Add(NewEffect);

	SetComponentTickEnabled(true);
	//UE_LOG(LogTemp, Warning, TEXT("Status Effect : %s Added for %f seconds"), *EffectClass->GetName(), Duration);
}

void UDEStatusEffectComponent::RemoveEffectsByTag(FGameplayTag Tag)
{
	// 태그가 비어있으면 무시
	if (!Tag.IsValid() || !OwnerActor.IsValid()) return;

	// 배열에서 먼저 빼고 OnRemove를 부른다 (TickComponent와 같은 이유)
	TArray<FActiveStatusEffect, TInlineAllocator<4>> Removed;
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		const FActiveStatusEffect& EffectData = ActiveEffects[i];
		if (EffectData.EffectDef && EffectData.EffectDef->StatusTags.HasTag(Tag))
		{
			Removed.Add(EffectData);
			ActiveEffects.RemoveAtSwap(i);
		}
	}

	for (FActiveStatusEffect& EffectData : Removed)
	{
		EffectData.EffectDef->OnRemove(OwnerActor.Get(), EffectData);
	}

	if (ActiveEffects.IsEmpty())
	{
		SetComponentTickEnabled(false);
	}
}

void UDEStatusEffectComponent::RemoveAllEffects()
{
	//UE_LOG(LogTemp, Warning, TEXT("Try RemoveAllEffects"));
	if (!OwnerActor.IsValid()) return;
	//UE_LOG(LogTemp, Warning, TEXT("RemoveAllEffects executed at least"));
	// 캐릭터 사망, 혹은 보스의 페이즈 전환(완전 정화) 패턴 시 호출됩니다.
	// 배열을 먼저 비우고 OnRemove를 부른다. 폭발 같은 OnRemove 안에서 AddEffect나 RemoveAllEffects가 다시 불려도 안전하다
	TArray<FActiveStatusEffect> Removed = MoveTemp(ActiveEffects);
	ActiveEffects.Reset();
	SetComponentTickEnabled(false);

	for (int32 i = Removed.Num() - 1; i >= 0; --i)
	{
		FActiveStatusEffect& EffectData = Removed[i];
		if (EffectData.EffectDef)
		{
			EffectData.EffectDef->OnRemove(OwnerActor.Get(), EffectData);
			UE_LOG(LogDarkEdenSurvivors, Verbose, TEXT("Status Effect : %s Removed by RemoveAllEffects"), *EffectData.EffectDef->GetName());
		}
	}

	if (ActiveEffects.IsEmpty())
	{
		// 풀링되는 몬스터가 다음에 쓸 수 있게 할당된 메모리를 돌려놓는다
		Removed.Reset();
		ActiveEffects = MoveTemp(Removed);
	}
	else
	{
		SetComponentTickEnabled(true);
	}
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

