// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ECaliberType.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
	FRotator ParticleRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gun)
	ECaliberType btype;

	virtual void BeginPlay() override;
public:
	AGun();
	virtual bool DoAttack() override;
	void Interact(TObjectPtr<class ABaseCharacter> TargetCharacter) override;
	int GetBulletsLeft();
	void SetBulletsLeft(int bullets);
	ECaliberType GetBulletType();
private:
	//bool bMalFunction;
	int curBulletsLeft;
};
