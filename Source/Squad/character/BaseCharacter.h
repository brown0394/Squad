// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../weapon/ECaliberType.h"
#include "../interface/UseGun.h"
#include "GenericTeamAgentInterface.h"
#include "BaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FBulletArr
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> innerArray;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStateChangeSignature);
UCLASS()
class SQUAD_API ABaseCharacter : public ACharacter, public IUseGun, public IGenericTeamAgentInterface
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

	//delegates used for to notify change to animation blueprint
	UPROPERTY(BlueprintAssignable, Category = "Character State")
	FOnStateChangeSignature OnAimingStateChange;
	UPROPERTY(BlueprintAssignable, Category = "Character State")
	FOnStateChangeSignature OnAttackingStateChange;
	UPROPERTY(BlueprintAssignable, Category = "Character State")
	FOnStateChangeSignature OnReloadStateChange;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Team")
	FGenericTeamId TeamId;

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Gun")
	TSubclassOf<class AGun> GunToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gun")
	TMap<ECaliberType, FBulletArr> Magazines;

	/** Called for aiming input */
	virtual void Aim() override;
	virtual void StopAiming() override;

	/** Called for use weapon input */
	virtual bool UseWeapon() override;

	virtual void Reload() override;

	bool IsAiming;
	bool IsAttacking;
	bool IsReloading;

	TObjectPtr<class AGun> CurGun;

	TObjectPtr<class UHealthComponent> HealthComp;

	virtual void Death();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	TObjectPtr<class AGun> GetCurGun();
	void SetCurGun(TObjectPtr<class AGun> gun);
};