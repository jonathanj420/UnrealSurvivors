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
	PrimaryComponentTick.bStartWithTickEnabled = false;

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

	// 안전한 삭제를 위해 무조건 역순(Reverse) 순회!
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		UDEStatusEffectBase* Effect = ActiveEffects[i];

		if (!Effect)
		{
			ActiveEffects.RemoveAtSwap(i);
			continue;
		}

		Effect->Tick(DeltaTime);

		// 지속 시간이 다 끝났다면?
		if (Effect->IsFinished())
		{
			Effect->OnRemove(); // (예: 깎았던 스탯 복구)
			OnEffectChanged.Broadcast(Effect, false); // UI에 해제 알림
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

	// 최적화: 쌩으로 NewObject를 때리기 전에 CDO를 가져와서 검사합니다.
	const UDEStatusEffectBase* CDO = GetDefault<UDEStatusEffectBase>(EffectClass);
	if (!CDO) return;

	// 1. 면역 체크
	if (ImmuneTags.Contains(CDO->EffectTag)) return;

	// 2. 중첩(Stack) 정책 처리
	if (CDO->EffectTag != EEffectTag::None)
	{
		UDEStatusEffectBase* Existing = GetEffectByTag(CDO->EffectTag);
		if (Existing)
		{
			switch (CDO->StackPolicy)
			{
			case EStackPolicy::Ignore:
				// 이미 걸려있으니 새 효과는 무시 (객체 생성 X)
				return;

			case EStackPolicy::Refresh:
				// 지속 시간만 초기화하고 위력(Power)은 더 쎈 놈으로 갱신
				Existing->ElapsedTime = 0.f;
				Existing->Power = FMath::Max(Existing->Power, Power);
				return;

			case EStackPolicy::Stack:
				// 최대 스택 수치 이하일 때만 중첩 증가
				if (Existing->CurrentStacks < CDO->MaxStacks)
				{
					Existing->CurrentStacks++;
					Existing->OnStacked(Existing->CurrentStacks);
				}
				return;

			case EStackPolicy::Replace:
				// 기존 효과를 지우고 아래 로직으로 내려가 새 효과를 씌웁니다.
				Existing->OnRemove();
				OnEffectChanged.Broadcast(Existing, false);
				ActiveEffects.Remove(Existing);
				break;
			}
		}
	}

	// 3. 필터를 모두 통과했다면 실제 객체 생성 및 적용!
	UDEStatusEffectBase* NewEffect = NewObject<UDEStatusEffectBase>(this, EffectClass);
	if (NewEffect)
	{
		NewEffect->InitEffect(Instigator, OwnerActor.Get(), Duration, Power, Interval);

		NewEffect->OnApply(); // 효과 시작 (예: 파티클 재생, 이속 감소)
		ActiveEffects.Add(NewEffect);

		OnEffectChanged.Broadcast(NewEffect, true); // UI에 부여 알림

		SetComponentTickEnabled(true);
	}
}

void UDEStatusEffectComponent::RemoveEffectsByTag(EEffectTag Tag)
{
	if (Tag == EEffectTag::None) return;

	// 역순 순회하며 일치하는 태그 전부 제거 (예: 해독 물약 먹었을 때 독 전부 제거)
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		UDEStatusEffectBase* Effect = ActiveEffects[i];
		if (Effect && Effect->EffectTag == Tag)
		{
			Effect->OnRemove();
			OnEffectChanged.Broadcast(Effect, false);
			ActiveEffects.RemoveAtSwap(i);
		}
	}
}

void UDEStatusEffectComponent::RemoveAllEffects()
{
	// 캐릭터 사망, 혹은 완전 정화 스킬 사용 시 싹 비워줍니다.
	for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
	{
		UDEStatusEffectBase* Effect = ActiveEffects[i];
		if (Effect)
		{
			Effect->OnRemove();
			OnEffectChanged.Broadcast(Effect, false);
		}
	}
	ActiveEffects.Empty();
}

bool UDEStatusEffectComponent::HasEffectWithTag(EEffectTag Tag) const
{
	if (Tag == EEffectTag::None) return false;

	// 람다식을 활용한 우아한 탐색
	return ActiveEffects.ContainsByPredicate([Tag](const UDEStatusEffectBase* Effect)
		{
			return Effect && Effect->EffectTag == Tag;
		});
}

UDEStatusEffectBase* UDEStatusEffectComponent::GetEffectByTag(EEffectTag Tag) const
{
	if (Tag == EEffectTag::None) return nullptr;

	UDEStatusEffectBase* const* FoundEffect = ActiveEffects.FindByPredicate([Tag](const UDEStatusEffectBase* Effect)
		{
			return Effect && Effect->EffectTag == Tag;
		});

	return FoundEffect ? *FoundEffect : nullptr;
}