// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "../character/BaseCharacter.h"

AGun::AGun() {
	
}

void AGun::BeginPlay() {
	Super::BeginPlay();
	curBulletsLeft = Ammo;
}

bool AGun::DoAttack() {
	
	// Try and fire a projectile
	if (Bullet != nullptr && IsReadyToAttack())
	{
		UWorld* const World = GetWorld();
		if (SM != nullptr)
		{
			timeSinceLastAttack = 0.0f;
			FVector muzzleLoc = SM->GetSocketLocation(FName(TEXT("MuzzleSocket")));
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
			ActorSpawnParams.bNoFail = false;
			FRotator rotation = GetActorRightVector().Rotation();
			
			// Spawn the projectile at the muzzle
			UGameplayStatics::SpawnEmitterAttached(ShootParticleSystem, SM, FName(TEXT("MuzzleSocket")), FVector::ZeroVector, ParticleRotation, FVector(ParticleSize));
			TObjectPtr<AProjectile> spawned = World->SpawnActor<AProjectile>(Bullet, muzzleLoc, rotation, ActorSpawnParams);
			if (spawned != nullptr) {
				spawned->SetDamage(damage); //nullptr means it couldnt be spawned
				spawned->SetInstigator(this);
			}
			--curBulletsLeft;
			return true;
		}
	}
	return false;
}

int AGun::GetBulletsLeft() {
	return curBulletsLeft;
}

void AGun::SetBulletsLeft(int bullets) {
	curBulletsLeft = bullets;
}

ECaliberType AGun::GetBulletType() {
	return btype;
}

void AGun::Interact(TObjectPtr<ABaseCharacter> TargetCharacter) {
	if (isAttached) return;
	TObjectPtr<AGun> CurGun = TargetCharacter->GetCurGun();
	if (CurGun != nullptr) {
		TObjectPtr<UWorld> const World = GetWorld();
		TObjectPtr<AGun> spawned =
			World->SpawnActor<AGun>(CurGun.GetClass(), TargetCharacter->GetActorLocation() + (TargetCharacter->GetActorForwardVector() * 20), FRotator::ZeroRotator);
		spawned->SetBulletsLeft(CurGun->GetBulletsLeft());
		CurGun->Destroy();
	}
	TargetCharacter->SetCurGun(this);
	AttachWeapon(TargetCharacter);
	isAttached = true;
}