// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"

AGun::AGun() {
	
}

void AGun::BeginPlay() {
	Super::BeginPlay();
}

void AGun::DoAttack() {
	
	// Try and fire a projectile
	if (Bullet != nullptr && IsReadyToAttack())
	{
		timeSinceLastAttack = 0.0f;
		UWorld* const World = GetWorld();
		if (World != nullptr && SM != nullptr)
		{
			FVector muzzleLoc = SM->GetSocketLocation(FName(TEXT("MuzzleSocket")));
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			FRotator rotation = GetActorRightVector().Rotation();
			// Spawn the projectile at the muzzle
			UGameplayStatics::SpawnEmitterAttached(ShootParticleSystem, SM, FName(TEXT("MuzzleSocket")), FVector::ZeroVector, ParticleRotation, FVector(ParticleSize));
			World->SpawnActor<AProjectile>(Bullet, muzzleLoc, rotation, ActorSpawnParams);
		}
	}
}