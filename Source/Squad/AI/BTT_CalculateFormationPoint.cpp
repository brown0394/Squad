// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CalculateFormationPoint.h"
#include "../character/SquadAIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

EBTNodeResult::Type UBTT_CalculateFormationPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	TObjectPtr<ASquadAIController> controller = Cast<ASquadAIController>(OwnerComp.GetAIOwner());
	if (controller == nullptr) return EBTNodeResult::Failed;

	TObjectPtr<UBlackboardComponent> blackboard = controller->GetBlackboardComponent();

	TObjectPtr<AActor> followTarget = Cast<AActor>(blackboard->GetValue<UBlackboardKeyType_Object>(controller->FollowTargetKeyID));
	if (followTarget == nullptr) return EBTNodeResult::Failed;

	FVector offset = blackboard->GetValue<UBlackboardKeyType_Vector>(controller->FormationOffsetKeyID);

	// Transform offset from player's local space to world space
	FVector worldOffset = followTarget->GetActorRotation().RotateVector(offset);
	FVector targetPoint = followTarget->GetActorLocation() + worldOffset;

	blackboard->SetValue<UBlackboardKeyType_Vector>(controller->PointOfInterestKeyID, targetPoint);
	return EBTNodeResult::Succeeded;
}
