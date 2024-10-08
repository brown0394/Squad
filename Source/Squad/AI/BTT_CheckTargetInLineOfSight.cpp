// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CheckTargetInLineOfSight.h"
#include "../character/SquadAIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

EBTNodeResult::Type UBTT_CheckTargetInLineOfSight::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	TObjectPtr<ASquadAIController> controller = Cast<ASquadAIController>(OwnerComp.GetAIOwner());
	TObjectPtr<UBlackboardComponent> blackboard = controller->GetBlackboardComponent();
	TObjectPtr<AActor> target = Cast<AActor>(blackboard->GetValue<UBlackboardKeyType_Object>(controller->TargetKeyID));
	if (target != nullptr && OwnerComp.GetAIOwner()->LineOfSightTo(target)) {
		blackboard->SetValue<UBlackboardKeyType_Bool>(controller->TargetOnSightID, true);
		return EBTNodeResult::Succeeded;
	}
	blackboard->SetValue<UBlackboardKeyType_Bool>(controller->TargetOnSightID, false);
	return EBTNodeResult::Failed;
}
