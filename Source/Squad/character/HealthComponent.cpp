// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent(): hp(100.0f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	GetOwner()->OnTakePointDamage.AddDynamic(this, &UHealthComponent::OnOwnerTakePointDamage);
	
}

void UHealthComponent::OnOwnerTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent,
	FName BoneName, FVector ShotFromDirection,
	const UDamageType* DamageType, AActor* DamageCauser) {
	FString bone = BoneName.ToString();

	if (bone.Len() < 2)
		return;

	switch (bone[0]) {
	case 'n' :
	case 'h': {
		if (bone[1] == 'e') Damage *= 1.5;
		else Damage *= 0.3;
		break;
	}
	case 's': { break; }
	default: { Damage *= 0.3; break; }
	}
	hp -= Damage;
	if (hp <= 0) {
		OnDeath.Broadcast();
		OnDeath.Clear();
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("damage : %f, cur hp : %f"), Damage, hp));
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, BoneName.ToString());
}