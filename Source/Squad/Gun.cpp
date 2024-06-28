// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Projectile.h"

AGun::AGun() {
	
}

void AGun::DoAttack() {
	// Try and fire a projectile
	if (Bullet != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr && SM != nullptr)
		{
			FTransform muzzle = SM->GetSocketTransform(FName(TEXT("b_gun_muzzleflash")));
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			World->SpawnActor<AProjectile>(Bullet, muzzle, ActorSpawnParams);
		}
	}
}