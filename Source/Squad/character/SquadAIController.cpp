// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

void ASquadAIController::OnPossess(APawn* InPawn) {
	if (InPawn == nullptr) return;
	Super::OnPossess(InPawn);
	Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    TargetKeyID = Blackboard->GetKeyID("Target");
    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, nullptr);
    TargetOnSightID = Blackboard->GetKeyID("TargetOnSight");
    Blackboard->SetValue<UBlackboardKeyType_Bool>(TargetOnSightID, false);
	BehaviorTreeComp->StartTree(*BehaviorTree);
    IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(InPawn);
    if (OwnerTeamAgent != nullptr) {
        SetGenericTeamId(OwnerTeamAgent->GetGenericTeamId());
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%d"), OwnerTeamAgent->GetGenericTeamId().GetId()));
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("%d"), TeamId.GetId()));
    }
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Posses");
}

ASquadAIController::ASquadAIController() {
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    
    // Sight Config
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 800.f;
    SightConfig->LoseSightRadius = 1200.f;
    SightConfig->PeripheralVisionAngleDegrees = 60.f;
    SightConfig->SetMaxAge(5.f);
    SightConfig->AutoSuccessRangeFromLastSeenLocation = -1.f;

    // Detect only specific actors 
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 500.f;
    HearingConfig->SetMaxAge(3.f);

    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    AIPerception->ConfigureSense(*HearingConfig);
    // ...

    AIPerception->OnPerceptionUpdated.AddDynamic(this, &ASquadAIController::PerceptionUpdated);
    AIPerception->OnTargetPerceptionForgotten.AddDynamic(this, &ASquadAIController::TargetForgotten);
    SetPerceptionComponent(*AIPerception);
}

void ASquadAIController::PerceptionUpdated(const TArray<AActor*>& UpdatedActors) {
    TObjectPtr<AActor> curTarget = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
    TObjectPtr<AActor> target = curTarget;
    TObjectPtr<APawn> owner = GetPawn();
    for (auto actor : UpdatedActors) {
        if (target == nullptr) {
            target = actor;
        }
        else if (owner->GetDistanceTo(actor) < owner->GetDistanceTo(target)) {
            target = actor;
        }
    }
    if (curTarget == nullptr || target.Get() != curTarget.Get()) {
        Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, target);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, owner->GetName());
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, target->GetName());
    }
}

void ASquadAIController::TargetForgotten(AActor* UpdatedActor) {
    TObjectPtr<AActor> target = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
    if (target == nullptr || UpdatedActor != target.Get()) return;
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, UpdatedActor->GetName());
    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, nullptr);
}

void ASquadAIController::Stop() {
    BehaviorTreeComp->StopLogic("Stop");
    AIPerception->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
    AIPerception->SetSenseEnabled(UAISense_Hearing::StaticClass(), false);
    GetPawn()->SetActorTickEnabled(false);
}

void ASquadAIController::SetGenericTeamId(const FGenericTeamId& InTeamID) {
    TeamId = InTeamID;
}
FGenericTeamId ASquadAIController::GetGenericTeamId() const { return TeamId; }