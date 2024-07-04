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
	UPROPERTY(EditDefaultsOnly, Category = Gun)
	TSubclassOf<class AProjectile> Bullet;
	UPROPERTY(EditDefaultsOnly, Category = Gun)
	TObjectPtr<UParticleSystem> ShootParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
	int Ammo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
	float ParticleSize;

	virtual void BeginPlay() override;
public:
	AGun();
	virtual void DoAttack() override;

private:
	//bool bMalFunction;
};
