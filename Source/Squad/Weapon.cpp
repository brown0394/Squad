// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "SquadCharacter.h"

// Sets default values
AWeapon::AWeapon()
{
	SM = CreateDefaultSubobject<UStaticMeshComponent>("DefaultMesh");
}

void AWeapon::AttachWeapon(TObjectPtr<ASquadCharacter> TargetCharacter) {
	if (TargetCharacter == NULL) return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	TObjectPtr<USkeletalMeshComponent> skeletalMC = TargetCharacter->GetComponentByClass<USkeletalMeshComponent>();
	if (skeletalMC != NULL) {
		AttachToComponent(skeletalMC, AttachmentRules, FName(TEXT("hand_r_rifle")));
	}
	
}