// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"
#include "../interface/UseGun.h"
#include "AIController.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	IUseGun* UseGun = Cast<IUseGun>(OwnerComp.GetAIOwner()->GetCharacter());
	if (UseGun == nullptr || !UseGun->UseWeapon()) return EBTNodeResult::Failed;
	return EBTNodeResult::Succeeded;
}