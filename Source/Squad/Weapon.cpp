// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "SquadCharacter.h"

// Sets default values
AWeapon::AWeapon() : timeSinceLastAttack(0.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	SM = CreateDefaultSubobject<UStaticMeshComponent>("DefaultMesh");
	SetActorEnableCollision(true);
	SM->SetSimulatePhysics(true);
}

void AWeapon::AttachWeapon(TObjectPtr<ASquadCharacter> TargetCharacter) {
	if (TargetCharacter == NULL) return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	TObjectPtr<USkeletalMeshComponent> skeletalMC = TargetCharacter->GetComponentByClass<USkeletalMeshComponent>();
	if (skeletalMC != NULL) {
		SetActorEnableCollision(false);
		SM->SetSimulatePhysics(false);
		AttachToComponent(skeletalMC, AttachmentRules, FName(TEXT("hand_r_rifle")));
	}
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	timeSinceLastAttack += DeltaTime;
}