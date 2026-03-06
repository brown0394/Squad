// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Aim.h"
#include "../character/SquadAIController.h"
#include "../interface/UseGun.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

EBTNodeResult::Type UBTT_Aim::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	TObjectPtr<ASquadAIController> controller = Cast<ASquadAIController>(OwnerComp.GetAIOwner());
	if (controller == nullptr) return EBTNodeResult::Failed;

	TObjectPtr<ACharacter> character = Cast<ACharacter>(controller->GetCharacter());
	if (character == nullptr) return EBTNodeResult::Failed;

	IUseGun* UseGun = Cast<IUseGun>(character);
	if (UseGun == nullptr) return EBTNodeResult::Failed;

	TObjectPtr<AActor> target = Cast<AActor>(
		controller->GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>(controller->TargetKeyID));

	if (target != nullptr) {
		controller->SetFocus(target);
		character->GetCharacterMovement()->bOrientRotationToMovement = false;
		character->GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}

	UseGun->Aim();
	return EBTNodeResult::Succeeded;
}