// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class SQUAD_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	virtual void DoAttack() PURE_VIRTUAL(AWeapon::DoAttack, ;);
	void AttachWeapon(TObjectPtr<class ASquadCharacter> TargetCharacter);
};
