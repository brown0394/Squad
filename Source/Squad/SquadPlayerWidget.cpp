// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadPlayerWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USquadPlayerWidget::CorsshairOnOff(bool on) {
	if (on) Crosshair->SetVisibility(ESlateVisibility::Visible);
	else Crosshair->SetVisibility(ESlateVisibility::Hidden);
}

void USquadPlayerWidget::InitializeText(FString& str) {
	TextOrderTo->SetText(FText::FromString(str));
	TextOrderTo->SetVisibility(ESlateVisibility::Hidden);
	Crosshair->SetVisibility(ESlateVisibility::Hidden);
	TextOrderList->SetVisibility(ESlateVisibility::Hidden);
}

void USquadPlayerWidget::TextOrderToOnOff(bool on) {
	if (on) TextOrderTo->SetVisibility(ESlateVisibility::Visible);
	else TextOrderTo->SetVisibility(ESlateVisibility::Hidden);
}

void USquadPlayerWidget::TextOrderListOnOff(bool on) {
	if (on) TextOrderList->SetVisibility(ESlateVisibility::Visible);
	else TextOrderList->SetVisibility(ESlateVisibility::Hidden);
}