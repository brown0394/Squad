// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_StopAiming.h"
#include "../interface/UseGun.h"
#include "AIController.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTT_StopAiming::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	IUseGun* UseGun = Cast<IUseGun>(OwnerComp.GetAIOwner()->GetCharacter());
	if (UseGun == nullptr) return EBTNodeResult::Failed;
	UseGun->StopAiming();
	return EBTNodeResult::Succeeded;
}
