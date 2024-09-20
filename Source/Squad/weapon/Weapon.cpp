// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "../character/BaseCharacter.h"

// Sets default values
AWeapon::AWeapon() : timeSinceLastAttack(0.0f), isAttached(false)
{
	PrimaryActorTick.bCanEverTick = true;
	SM = CreateDefaultSubobject<UStaticMeshComponent>("DefaultMesh");
	SetActorEnableCollision(true);
	SM->SetSimulatePhysics(true);
}

void AWeapon::AttachWeapon(TObjectPtr<ABaseCharacter> TargetCharacter) {
	if (TargetCharacter == NULL) return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	TObjectPtr<USkeletalMeshComponent> skeletalMC = TargetCharacter->GetComponentByClass<USkeletalMeshComponent>();
	if (skeletalMC != NULL) {
		SetActorEnableCollision(false);
		SM->SetSimulatePhysics(false);
		AttachToComponent(skeletalMC, AttachmentRules, FName(TEXT("hand_r_rifle")));
		isAttached = true;
	}
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	timeSinceLastAttack += DeltaTime;
}

bool AWeapon::IsReadyToAttack() {
	return timeSinceLastAttack >= attackRate;
}

void AWeapon::Interact(TObjectPtr<class ABaseCharacter> TargetCharacter) {
	if (isAttached) return;
	AttachWeapon(TargetCharacter);
}