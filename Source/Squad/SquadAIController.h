// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SquadAIController.generated.h"

/**
 * 
 */
UCLASS()
class SQUAD_API ASquadAIController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void OnPossess(APawn* InPawn) override;
	ASquadAIController();
	UPROPERTY()
	TObjectPtr<class UBehaviorTreeComponent> BehaviorTreeComp;
	UPROPERTY()
	TObjectPtr<class UBlackboardComponent> BlackboardComp;

};
