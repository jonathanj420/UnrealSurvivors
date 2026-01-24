// Fill out your copyright notice in the Description page of Project Settings.


#include "DEBehavior_PlayNiagara.h"
#include "NiagaraFunctionLibrary.h" // 필수
#include "NiagaraComponent.h"
#include "DESkillContext.h"
#include "GameFramework/Actor.h"

void UDEBehavior_PlayNiagara::Execute(FDESkillContext& Context)
{
	if (!NiagaraAsset) return;

	TArray<FVector> SpawnLocations;
	TArray<AActor*> AttachTargets;

	// Switch문에서 바로 사용
	switch (SpawnLocation)
	{
	case EEffectSpawnLocation::Instigator:
		if (Context.Instigator)
		{
			SpawnLocations.Add(Context.Instigator->GetActorLocation());
			if (bAttachToActor) AttachTargets.Add(Context.Instigator);
		}
		break;

	case EEffectSpawnLocation::TargetActors:
		for (AActor* Target : Context.Targets)
		{
			if (Target)
			{
				SpawnLocations.Add(Target->GetActorLocation());
				if (bAttachToActor) AttachTargets.Add(Target);
			}
		}
		break;

	case EEffectSpawnLocation::CustomLocations:
		for (const FVector& Loc : Context.CustomLocations)
		{
			SpawnLocations.Add(Loc);
		}
		break;
	}

	// 실제 재생 (아까와 동일)
	for (int32 i = 0; i < SpawnLocations.Num(); ++i)
	{
		FVector FinalPos = SpawnLocations[i] + Offset;
		UNiagaraComponent* SpawnedComp = nullptr;

		if (bAttachToActor && AttachTargets.IsValidIndex(i))
		{
			SpawnedComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				NiagaraAsset,
				AttachTargets[i]->GetRootComponent(),
				NAME_None,
				Offset,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true
			);
		}
		else
		{
			SpawnedComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				NiagaraAsset,
				FinalPos
			);
		}

		// 2. ★ 크기 주입 (범용성 UP)
		// SizeVariableName이 설정되어 있다면 (예: "User.EffectSize")
		// 현재 스킬의 Radius 값(450 등)을 거기에 넣어준다.
		if (SpawnedComp && !SizeVariableName.IsNone())
		{
			// 1. 기본값은 Context의 Radius (데이터 테이블 값)
			float BaseSize = Context.Radius;

			// (예외처리) 만약 Radius가 0인 스킬(데이터 없음)이라면?
			// 그냥 1.0이나 적당한 기본값을 써서 Multiplier만으로 조절되게 함
			if (BaseSize <= KINDA_SMALL_NUMBER)
			{
				BaseSize = 100.0f; // 혹은 1.0f, 에셋에 따라 다름
			}

			// 2. 최종값 계산 (데이터 x 보정값)
			float FinalSize = BaseSize * SizeMultiplier;

			// 3. 나이아가라에 주입
			SpawnedComp->SetVariableFloat(SizeVariableName, FinalSize);
		}
		else if (SpawnedComp && SizeMultiplier != 1.0f)
		{
			// ★ 보너스 기능:
			// 만약 "User.EffectSize" 변수명을 안 적었더라도, 
			// Multiplier가 1.0이 아니면 -> 컴포넌트 자체를 스케일링 해버림 (3D Scale)
			SpawnedComp->SetWorldScale3D(FVector(SizeMultiplier));
		}

	}
}
