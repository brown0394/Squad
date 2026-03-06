// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_StopAiming.h"
#include "../interface/UseGun.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UBTT_StopAiming::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	TObjectPtr<ACharacter> character = Cast<ACharacter>(OwnerComp.GetAIOwner()->GetCharacter());
	if (character == nullptr) return EBTNodeResult::Failed;

	IUseGun* UseGun = Cast<IUseGun>(character);
	if (UseGun == nullptr) return EBTNodeResult::Failed;

	OwnerComp.GetAIOwner()->ClearFocus(EAIFocusPriority::Gameplay);
	character->GetCharacterMovement()->bOrientRotationToMovement = true;
	character->GetCharacterMovement()->bUseControllerDesiredRotation = false;

	UseGun->StopAiming();
	return EBTNodeResult::Succeeded;
}
