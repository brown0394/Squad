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
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "HealthComponent.h"
#include "BaseCharacter.h"

void ASquadAIController::OnPossess(APawn* InPawn) {
	if (InPawn == nullptr) 
        return;

	Super::OnPossess(InPawn);
	Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    TargetKeyID = Blackboard->GetKeyID("Target");
    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, nullptr);
    TargetOnSightID = Blackboard->GetKeyID("TargetOnSight");
    Blackboard->SetValue<UBlackboardKeyType_Bool>(TargetOnSightID, false);
    PerceptionCauserLocID = Blackboard->GetKeyID("PerceptionCauserLoc");
    FollowTargetKeyID = Blackboard->GetKeyID("FollowTarget");
    Blackboard->SetValue<UBlackboardKeyType_Object>(FollowTargetKeyID, nullptr);
    FormationOffsetKeyID = Blackboard->GetKeyID("FormationOffset");
    PointOfInterestKeyID = Blackboard->GetKeyID("PointOfInterest");
    IsOrderedMoveToPositionKeyID = Blackboard->GetKeyID("IsOrderedMoveToPosition");

	BehaviorTreeComp->StartTree(*BehaviorTree);

    IGenericTeamAgentInterface* OwnerTeamAgent = Cast<IGenericTeamAgentInterface>(InPawn);
    if (OwnerTeamAgent != nullptr) {
        SetGenericTeamId(OwnerTeamAgent->GetGenericTeamId());
    }
}

ASquadAIController::ASquadAIController() : bTargetDesignated(false), bFollowingLeader(false) {
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
    if (!stimulus.IsValid() || bTargetDesignated || bFollowingLeader)
        return;

    TObjectPtr<AActor> curTarget = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
    
    static constexpr uint8 stimiulusSight = 0;
    static constexpr uint8 stimiulusTouch = 1;
    static constexpr uint8 stimiulusHear = 2;

    switch (stimulus.Type.Index) {
        case stimiulusSight: {
            OnStimulusSight(curTarget, UpdatedActor);
        } break;
        case stimiulusTouch: {  }
        case stimiulusHear: {
            if (curTarget == nullptr)
                Blackboard->SetValue<UBlackboardKeyType_Vector>(PerceptionCauserLocID, UpdatedActor->GetActorLocation());
            
        }break;
    }
}

void ASquadAIController::OnStimulusSight( const TObjectPtr<AActor> curTarget, AActor* UpdatedActor )
{
    TObjectPtr<AActor> target = curTarget;
    if ( target == nullptr )
        target = UpdatedActor;
    else if ( changeTargetSeenByDistance(target, UpdatedActor) == false )
        return;

     BindTargetOnDeath(curTarget, target);
     Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, target);
}

bool ASquadAIController::changeTargetSeenByDistance(TObjectPtr<AActor>& CurTarget, TObjectPtr<AActor> ActorSensed) {
    if (CurTarget.Get() == ActorSensed.Get())
        return false;

    TObjectPtr<APawn> owner = GetPawn();
    if (owner->GetDistanceTo(ActorSensed) < owner->GetDistanceTo(CurTarget)) {
        CurTarget = ActorSensed;
        return true;
    }

    return false;
}

void ASquadAIController::LookAtSenseOrigin(TObjectPtr<AActor> ActorSensed) {
    TObjectPtr<APawn> owner = GetPawn();
    owner->FaceRotation((ActorSensed->GetActorLocation() - owner->GetActorLocation()).ToOrientationRotator());
}

void ASquadAIController::TargetForgotten(AActor* UpdatedActor) {
    TObjectPtr<AActor> target = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
    if (target == nullptr || UpdatedActor != target.Get()) return;
    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, nullptr);
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
    bTargetDesignated = false;
}

void ASquadAIController::DesignateTarget(TObjectPtr<AActor> Target) {
    if ( ( Target == nullptr ) || ( Cast<AActor>(GetPawn()) == Target ) )
        return;

    bTargetDesignated = true;
    bFollowingLeader = false;
    TObjectPtr<AActor> curTarget = Cast<AActor>(Blackboard->GetValue<UBlackboardKeyType_Object>(TargetKeyID));
    BindTargetOnDeath(curTarget, Target);
    Blackboard->SetValue<UBlackboardKeyType_Object>(FollowTargetKeyID, nullptr);
    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, Target);
}

void ASquadAIController::FollowSquadLeader(FVector FormationOffset) {
    if (SquadLeader == nullptr) return;

    bFollowingLeader = true;
    bTargetDesignated = false;

    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, nullptr);
    Blackboard->SetValue<UBlackboardKeyType_Object>(FollowTargetKeyID, SquadLeader);
    Blackboard->SetValue<UBlackboardKeyType_Vector>(FormationOffsetKeyID, FormationOffset);
}

void ASquadAIController::MoveToPosition(FVector Position) {
	bFollowingLeader = false;
	bTargetDesignated = false;

	Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, nullptr);
	Blackboard->SetValue<UBlackboardKeyType_Object>(FollowTargetKeyID, nullptr);
	Blackboard->SetValue<UBlackboardKeyType_Vector>(PointOfInterestKeyID, Position);
    Blackboard->SetValue<UBlackboardKeyType_Bool>(IsOrderedMoveToPositionKeyID, true);
}

void ASquadAIController::FreeWill() {
    bFollowingLeader = false;
    bTargetDesignated = false;

    Blackboard->SetValue<UBlackboardKeyType_Object>(FollowTargetKeyID, nullptr);
    Blackboard->SetValue<UBlackboardKeyType_Object>(TargetKeyID, nullptr);
    Blackboard->SetValue<UBlackboardKeyType_Bool>(IsOrderedMoveToPositionKeyID, false);
}