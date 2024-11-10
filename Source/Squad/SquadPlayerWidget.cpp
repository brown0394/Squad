// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadPlayerWidget.h"
#include "Components/Image.h"

void USquadPlayerWidget::CorsshairOnOff(bool on) {
	if (on) Crosshair->SetVisibility(ESlateVisibility::Visible);
	else Crosshair->SetVisibility(ESlateVisibility::Hidden);
}