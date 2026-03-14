// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetRandomShotCount.h"
#include "../character/SquadAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"

EBTNodeResult::Type UBTT_SetRandomShotCount::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	ASquadAIController* Controller = Cast<ASquadAIController>(OwnerComp.GetAIOwner());
	if (Controller == nullptr) return EBTNodeResult::Failed;

	int32 ShotCount = FMath::RandRange(0, 4);
	OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Int>(Controller->ShotsToFireKeyID, ShotCount);
	return EBTNodeResult::Succeeded;
}
