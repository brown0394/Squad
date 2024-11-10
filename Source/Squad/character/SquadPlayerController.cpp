// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadPlayerController.h"
#include "../SquadPlayerWidget.h"

void ASquadPlayerController::BeginPlay() {
	Super::BeginPlay();

	if (SBWidgetClass) {
		sbWidget = CreateWidget<USquadPlayerWidget>(this, SBWidgetClass);
		
		sbWidget->AddToViewport();
	}
}

void ASquadPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (sbWidget) {
		sbWidget->RemoveFromParent();
		sbWidget = nullptr;
	}
	if (SBWidgetClass) {
		SBWidgetClass = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}