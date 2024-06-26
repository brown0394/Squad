// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

/**
 * 
 */
UCLASS()
class SQUAD_API AGun : public AWeapon
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gun)
	int ammo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gun)
	int damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gun)
	float fireRate;

	UPROPERTY(EditDefaultsOnly, Category = Gun)
	TSubclassOf<class AProjectile> Bullet;
public:
	AGun();
	virtual void DoAttack() override;
private:
	//bool bMalFunction;
};
