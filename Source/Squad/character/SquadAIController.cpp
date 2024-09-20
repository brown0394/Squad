// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AICharacter.h"

void ASquadAIController::OnPossess(APawn* InPawn) {
	if (InPawn == nullptr) return;
	Super::OnPossess(InPawn);
	BlackboardComp->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	
	BehaviorTreeComp->StartTree(*BehaviorTree);
}

ASquadAIController::ASquadAIController() {
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
}