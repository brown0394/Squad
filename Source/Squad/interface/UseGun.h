// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UseGun.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UUseGun : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SQUAD_API IUseGun
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Aim();
	virtual bool UseWeapon();
	virtual void Reload();
};
