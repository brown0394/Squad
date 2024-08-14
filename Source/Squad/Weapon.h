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
	virtual bool DoAttack() PURE_VIRTUAL(AWeapon::DoAttack, return true;);
	void AttachWeapon(TObjectPtr<class ABaseCharacter> TargetCharacter);
	virtual void Tick(float DeltaTime) override;
	bool IsReadyToAttack();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> SM;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	int damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float attackRate;

	float timeSinceLastAttack;
};
