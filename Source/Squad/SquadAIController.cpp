// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AICharacter.h"

void ASquadAIController::OnPossess(APawn* InPawn) {
	if (InPawn == nullptr) return;
	Super::OnPossess(InPawn);
	TObjectPtr<AAICharacter> AiChar = Cast<AAICharacter>(InPawn);
	if (AiChar == nullptr) return;
	BlackboardComp->InitializeBlackboard(*AiChar->BehaviorTree->BlackboardAsset);
	
	BehaviorTreeComp->StartTree(*AiChar->BehaviorTree);
}

ASquadAIController::ASquadAIController() {
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
}