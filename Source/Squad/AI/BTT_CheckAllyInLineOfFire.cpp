// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CheckAllyInLineOfFire.h"
#include "../character/SquadAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "GenericTeamAgentInterface.h"

EBTNodeResult::Type UBTT_CheckAllyInLineOfFire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	TObjectPtr<ASquadAIController> controller = Cast<ASquadAIController>(OwnerComp.GetAIOwner());
	if (controller == nullptr) return EBTNodeResult::Failed;

	TObjectPtr<UBlackboardComponent> blackboard = controller->GetBlackboardComponent();
	TObjectPtr<AActor> target = Cast<AActor>(blackboard->GetValue<UBlackboardKeyType_Object>(controller->TargetKeyID));
	if (target == nullptr) return EBTNodeResult::Failed;

	TObjectPtr<APawn> ownerPawn = controller->GetPawn();
	if (ownerPawn == nullptr) return EBTNodeResult::Failed;

	FVector start = ownerPawn->GetPawnViewLocation();
	FVector end = target->GetActorLocation();

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(ownerPawn);
	queryParams.AddIgnoredActor(target);

	TArray<FHitResult> hits;
	bool bHit = ownerPawn->GetWorld()->LineTraceMultiByChannel(
		hits, start, end, ECC_Pawn, queryParams
	);

	if (bHit) {
		FGenericTeamId ownerTeam = controller->GetGenericTeamId();
		for (const FHitResult& hit : hits) {
			TObjectPtr<AActor> hitActor = hit.GetActor();
			if (hitActor == nullptr) continue;

			IGenericTeamAgentInterface* teamAgent = Cast<IGenericTeamAgentInterface>(hitActor);
			if (teamAgent != nullptr && teamAgent->GetGenericTeamId() == ownerTeam) {
				return EBTNodeResult::Failed;
			}
		}
	}

	return EBTNodeResult::Succeeded;
}
