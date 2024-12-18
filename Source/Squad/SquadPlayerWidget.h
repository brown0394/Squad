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

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextOrderTo;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TextOrderList;

	void CorsshairOnOff(bool on);
	void TextOrderToOnOff(bool on);
	void TextOrderListOnOff(bool on);
	void InitializeText(FString& str);
};
