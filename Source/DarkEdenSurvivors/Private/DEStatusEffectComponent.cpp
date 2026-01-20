// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffectComponent.h"
#include "DEMonsterBase.h"


// Sets default values for this component's properties
UDEStatusEffectComponent::UDEStatusEffectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDEStatusEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDEStatusEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 역순 순회 (삭제 시 안전하게)
    for (int32 i = ActiveEffects.Num() - 1; i >= 0; --i)
    {
        UDEStatusEffectBase* Effect = ActiveEffects[i];
        if (Effect)
        {
            Effect->OnTick(DeltaTime);

            if (Effect->IsFinished())
            {
                Effect->OnRemove();
                ActiveEffects.RemoveAtSwap(i); // 종료된 효과 삭제
            }
        }
    }
}

void UDEStatusEffectComponent::AddEffect(TSubclassOf<UDEStatusEffectBase> EffectClass, AActor* Instigator, float Duration, float Power, float Interval)
{
    if (!EffectClass) return;

    if (!OwnerMonster) OwnerMonster = Cast<ADEMonsterBase>(GetOwner());
    if (!OwnerMonster) return;

    // 중복 처리 로직 (TODO: 같은 종류가 있으면 시간만 갱신할지, 중첩시킬지)
    // 여기선 심플하게 그냥 추가하는 걸로 구현

    UDEStatusEffectBase* NewEffect = NewObject<UDEStatusEffectBase>(this, EffectClass);
    NewEffect->InitEffect(Instigator, OwnerMonster, Duration, Power, Interval);
    NewEffect->OnApply(); // 적용 시작!

    ActiveEffects.Add(NewEffect);
}

void UDEStatusEffectComponent::RemoveAllEffects()
{
    for (UDEStatusEffectBase* Effect : ActiveEffects)
    {
        if (Effect) Effect->OnRemove();
    }
    ActiveEffects.Empty();
}