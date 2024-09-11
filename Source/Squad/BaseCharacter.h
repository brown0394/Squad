// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ECaliberType.h"
#include "BaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FBulletArr
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> innerArray;
};

UCLASS()
class SQUAD_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	bool GetIsAiming();

	UFUNCTION(BlueprintCallable)
	bool GetIsAttacking();

	UFUNCTION(BlueprintCallable)
	void StopAttacking();

	UFUNCTION(BlueprintCallable)
	bool GetIsReloading();

	UFUNCTION(BlueprintCallable)
	void ReloadingDone();

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Gun")
	TSubclassOf<class AGun> GunToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gun")
	TMap<ECaliberType, FBulletArr> Magazines;

	/** Called for aiming input */
	void Aim();
	void StopAiming();

	/** Called for use weapon input */
	void UseWeapon();

	void Reload();

	bool IsAiming;
	bool IsAttacking;
	bool IsReloading;

	TObjectPtr<class AGun> CurGun;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	TObjectPtr<class AGun> GetCurGun();
	void SetCurGun(TObjectPtr<class AGun> gun);
};