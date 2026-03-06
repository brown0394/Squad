// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_CheckAllyInLineOfFire.generated.h"

/**
 * Checks if any friendly actor is between the AI and its target.
 * Returns Failed if an ally is in the line of fire (AI should hold fire).
 * Returns Succeeded if the path is clear to shoot.
 */
UCLASS()
class SQUAD_API UBTT_CheckAllyInLineOfFire : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
