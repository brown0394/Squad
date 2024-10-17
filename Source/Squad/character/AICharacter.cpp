// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "SquadAIController.h"

void AAICharacter::Death() {
	Super::Death();
	TObjectPtr<ASquadAIController> AIController = Cast<ASquadAIController>(GetController());
	AIController->Stop();
}