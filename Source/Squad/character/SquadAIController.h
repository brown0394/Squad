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
	FBlackboard::FKey PerceptionCauserLocID;
	FBlackboard::FKey FollowTargetKeyID;
	FBlackboard::FKey FormationOffsetKeyID;
	FBlackboard::FKey PointOfInterestKeyID;
	FBlackboard::FKey IsOrderedMoveToPositionKeyID;
	void Stop();

	FGenericTeamId TeamId;

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	TObjectPtr<AActor> SquadLeader;

	void DesignateTarget(TObjectPtr<AActor> Target);
	void FollowSquadLeader(FVector FormationOffset);
	void MoveToPosition(FVector Position);
	void FreeWill();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UAIPerceptionComponent> AIPerception;

	TObjectPtr<class UAISenseConfig_Sight> SightConfig;
	TObjectPtr<class UAISenseConfig_Hearing> HearingConfig;
	TObjectPtr<class UAISenseConfig_Damage> DamageSenseConfig;
	UFUNCTION()
	void PerceptionUpdated(AActor* UpdatedActor, FAIStimulus stimulus);
	UFUNCTION()
	void TargetForgotten(AActor* UpdatedActor);
	
	FDelegateHandle TargetOnDeathHandle;

	void TargetDeath();
private:

	bool changeTargetSeenByDistance(TObjectPtr<AActor>& CurTarget, TObjectPtr<AActor> ActorSensed);
	void LookAtSenseOrigin(TObjectPtr<AActor> ActorSensed);
	void BindTargetOnDeath(TObjectPtr<AActor> CurTarget, TObjectPtr<AActor> ActorSensed);
	void OnStimulusSight( const TObjectPtr<AActor> curTarget, AActor* UpdatedActor );

	bool bTargetDesignated;
	bool bFollowingLeader;
};
