// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SquadPlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class SQUAD_API USquadPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<class UImage> Crosshair;

	void CorsshairOnOff(bool on);
};
