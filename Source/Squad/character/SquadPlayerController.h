// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SquadPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SQUAD_API ASquadPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class USquadPlayerWidget> SBWidgetClass;

	void CorsshairOnOff(bool on);
protected:
	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

	
private:
	UPROPERTY()
	TObjectPtr<class USquadPlayerWidget> sbWidget;
};
