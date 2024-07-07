// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"


class USphereComponent;
class UProjectileMovementComponent;
UCLASS()
class SQUAD_API AProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
protected:
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TObjectPtr<UParticleSystem> ProjectileParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float ParticleSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	FRotator ParticleRotation;

public:	
	// Sets default values for this actor's properties
	AProjectile();
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};
