// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_CalculateFormationPoint.generated.h"

/**
 * Calculates world position from player location + rotation + formation offset
 */
UCLASS()
class SQUAD_API UBTT_CalculateFormationPoint : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
