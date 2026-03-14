// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"
#include "../interface/UseGun.h"
#include "../character/SquadAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	ASquadAIController* Controller = Cast<ASquadAIController>(OwnerComp.GetAIOwner());
	if (Controller == nullptr) return EBTNodeResult::Failed;

	IUseGun* UseGun = Cast<IUseGun>(Controller->GetCharacter());
	if (UseGun == nullptr || !UseGun->UseWeapon()) return EBTNodeResult::Failed;

	TObjectPtr<UBlackboardComponent> Blackboard = OwnerComp.GetBlackboardComponent();
	Blackboard->SetValue<UBlackboardKeyType_Float>(Controller->WeaponCoolDownID, UseGun->GetFireRate() );

	int32 ShotsToFire = Blackboard->GetValue<UBlackboardKeyType_Int>(Controller->ShotsToFireKeyID);
	if (ShotsToFire <= 0) return EBTNodeResult::Succeeded;

	Blackboard->SetValue<UBlackboardKeyType_Int>(Controller->ShotsToFireKeyID, ShotsToFire - 1);
	return EBTNodeResult::Succeeded;
}