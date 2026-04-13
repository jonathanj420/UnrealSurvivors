// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffect_DelayedAction.h"

UDEStatusEffect_DelayedAction::UDEStatusEffect_DelayedAction()
{
	StackPolicy = EStackPolicy::Ignore;
	MaxStacks = 1;
}

void UDEStatusEffect_DelayedAction::OnRemove(AActor* Target, FActiveStatusEffect& EffectData) const
{

    if (Target)
    {
        ExecuteAction(Target, EffectData);
    }

    Super::OnRemove(Target, EffectData);

    
}
