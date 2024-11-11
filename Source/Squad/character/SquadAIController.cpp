// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Damage.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "HealthComponent.h"
#include "BaseCharacter.h"

void ASquadAIController::OnPossess(APawn* InPawn) {
	if (InPawn == nullptr) return;
	Super::OnPossess(InPawn);
	Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    TargetKeyID = Blackboard->GetKeyID("Target");
    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, nullptr);
    TargetOnSightID = Blackboard->GetKeyID("TargetOnSight");
    Blackboard->SetValue<UBlackboardKeyType_Bool>(TargetOnSightID, false);
    HasTargetID = Blackboard->GetKeyID("HasTarget");
    Blackboard->SetValue<UBlackboardKeyType_Bool>(HasTargetID, false);
	BehaviorTreeComp->StartTree(*BehaviorTree);

    IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(InPawn);
    if (OwnerTeamAgent != nullptr) {
        SetGenericTeamId(OwnerTeamAgent->GetGenericTeamId());
    }
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
    SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 1000.f;
    HearingConfig->SetMaxAge(3.f);
    
    HearingConfig->DetectionByAffiliation.bDetectEnemies = false;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
    AIPerception->ConfigureSense(*HearingConfig);

    DamageSenseConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
    DamageSenseConfig->SetMaxAge(1.0f);
    AIPerception->ConfigureSense(*DamageSenseConfig);
    // ...

    AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ASquadAIController::PerceptionUpdated);
    AIPerception->OnTargetPerceptionForgotten.AddDynamic(this, &ASquadAIController::TargetForgotten);
    SetPerceptionComponent(*AIPerception);
}

void ASquadAIController::PerceptionUpdated(AActor* UpdatedActor, FAIStimulus stimulus) {
    if (!stimulus.IsValid()) return;
    TObjectPtr<AActor> curTarget = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
    TObjectPtr<AActor> target = curTarget;
    bool bHasTarget = Blackboard->GetValue<UBlackboardKeyType_Bool>(HasTargetID);
    
    switch (stimulus.Type.Index) {
        case 0: {
            if (bHasTarget == false) {
                Blackboard->SetValue<UBlackboardKeyType_Bool>(HasTargetID, true);
                target = UpdatedActor;
            }
            else TargetSeen(target, UpdatedActor); 
            if (target.Get() != curTarget.Get()) {
                BindTargetOnDeath(curTarget, target);
                Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, target);
            }
            break; 
        }
        case 1: {  }
        case 2: { 
            if (bHasTarget == false)
                LookAtSenseOrigin(UpdatedActor); 
            break; 
        }
    }
}

void ASquadAIController::TargetSeen(TObjectPtr<AActor>& CurTarget, TObjectPtr<AActor> ActorSensed) {
    if (CurTarget.Get() == ActorSensed.Get()) return;
    TObjectPtr<APawn> owner = GetPawn();
    if (owner->GetDistanceTo(ActorSensed) < owner->GetDistanceTo(CurTarget)) {
        CurTarget = ActorSensed;
    }
}
void ASquadAIController::LookAtSenseOrigin(TObjectPtr<AActor> ActorSensed) {
    TObjectPtr<APawn> owner = GetPawn();
    owner->FaceRotation((ActorSensed->GetActorLocation() - owner->GetActorLocation()).ToOrientationRotator());
}

void ASquadAIController::TargetForgotten(AActor* UpdatedActor) {
    TObjectPtr<AActor> target = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
    if (target == nullptr || UpdatedActor != target.Get()) return;
    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, nullptr);
    Blackboard->SetValue<UBlackboardKeyType_Bool>(HasTargetID, false);
    TObjectPtr<ABaseCharacter> BaseCharacter = Cast<ABaseCharacter>(target);
    if (BaseCharacter != nullptr) {
        BaseCharacter->GetHealthComponent()->OnDeath.Remove(TargetOnDeathHandle);
    }
}

void ASquadAIController::BindTargetOnDeath(TObjectPtr<AActor> CurTarget, TObjectPtr<AActor> ActorSensed) {
    TObjectPtr<ABaseCharacter> BaseCharacter;
    if (CurTarget != nullptr) {
        BaseCharacter = Cast<ABaseCharacter>(CurTarget);
        if (BaseCharacter != nullptr) {
            BaseCharacter->GetHealthComponent()->OnDeath.Remove(TargetOnDeathHandle);
        }
    }
    BaseCharacter = Cast<ABaseCharacter>(ActorSensed);
    if (BaseCharacter != nullptr) {
        TargetOnDeathHandle = BaseCharacter->GetHealthComponent()->OnDeath.AddUObject(this, &ASquadAIController::TargetDeath);
    }
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

void ASquadAIController::TargetDeath() {
    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, nullptr);
    Blackboard->SetValue<UBlackboardKeyType_Bool>(HasTargetID, false);
}