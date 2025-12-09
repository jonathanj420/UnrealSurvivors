// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "ABAIController.h"
#include "TestCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn");

}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto TestCharacter = Cast<ATestCharacter>(OwnerComp.GetAIOwner()->GetPawn()); //이 컴포의 ai의 폰을 TestCharacter로, 추후 TestPawn등 몹으로
	if (nullptr == TestCharacter)
		return EBTNodeResult::Failed;

	auto Target = Cast<ATestCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AABAIController::TargetKey));
	if (nullptr == Target)
		return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - TestCharacter->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	TestCharacter->SetActorRotation(FMath::RInterpTo(TestCharacter->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 2.0f)); //이 캐릭터의 로테이션을 타겟롯으로

	return EBTNodeResult::Succeeded;

}
