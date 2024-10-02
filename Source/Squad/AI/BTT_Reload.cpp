// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Reload.h"
#include "../interface/UseGun.h"
#include "AIController.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTT_Reload::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	IUseGun* UseGun = Cast<IUseGun>(OwnerComp.GetAIOwner()->GetCharacter());
	if (UseGun == nullptr) return EBTNodeResult::Failed;
	UseGun->Reload();
	return EBTNodeResult::Succeeded;
}