// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GenericTeamAgentInterface.h"
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
	UPROPERTY(EditAnywhere, Category = "BehaviorTree")
	TObjectPtr<class UBehaviorTree> BehaviorTree;
	FBlackboard::FKey TargetKeyID;
	FBlackboard::FKey TargetOnSightID;
	void Stop();

	FGenericTeamId TeamId;

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UAIPerceptionComponent> AIPerception;

	TObjectPtr<class UAISenseConfig_Sight> SightConfig;
	TObjectPtr<class UAISenseConfig_Hearing> HearingConfig;
	TObjectPtr<class UAISenseConfig_Damage> DamageSenseConfig;
	UFUNCTION()
	void PerceptionUpdated(const TArray<AActor*>& UpdatedActors);
	UFUNCTION()
	void TargetForgotten(AActor* UpdatedActor);
	
	FDelegateHandle TargetOnDeathHandle;

	void TargetDeath();
};
