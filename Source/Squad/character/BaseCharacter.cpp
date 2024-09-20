// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../weapon/Gun.h"

// Sets default values
ABaseCharacter::ABaseCharacter() : IsAiming(false), IsAttacking(false), IsReloading(false)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	TObjectPtr<UWorld> const World = GetWorld();
	World->SpawnActor<AGun>(GunToSpawn, FVector::ZeroVector, FRotator::ZeroRotator)->Interact(this);
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::Aim() {
	if (!IsAiming) {
		IsAiming = true;
		GetCharacterMovement()->MaxWalkSpeed = 250.f;
		OnAimingStateChange.Broadcast();
	}
}

void ABaseCharacter::StopAiming() {
	if (IsAiming) {
		IsAiming = false;
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
		OnAimingStateChange.Broadcast();
	}
}

void ABaseCharacter::UseWeapon() {
	if (CurGun == nullptr || IsReloading || IsAttacking) return;
	if (CurGun->DoAttack()) {
		IsAttacking = true;
		OnAttackingStateChange.Broadcast();
	}
}

bool ABaseCharacter::GetIsAiming() {
	return IsAiming;
}

bool ABaseCharacter::GetIsAttacking() {
	return IsAttacking;
}

void ABaseCharacter::Reload() {
	if (CurGun == nullptr || IsReloading) return;
	if (!Magazines[CurGun->GetBulletType()].innerArray.IsEmpty()) {
		IsAttacking = false;
		IsReloading = true;
		OnAttackingStateChange.Broadcast();
		OnReloadStateChange.Broadcast();
	}
}

bool ABaseCharacter::GetIsReloading() {
	return IsReloading;
}

void ABaseCharacter::StopAttacking() {
	IsAttacking = false;
}

void ABaseCharacter::ReloadingDone() {
	IsReloading = false;
	CurGun->SetBulletsLeft(Magazines[CurGun->GetBulletType()].innerArray.Pop());
}

TObjectPtr<AGun> ABaseCharacter::GetCurGun() { return CurGun; };
void ABaseCharacter::SetCurGun(TObjectPtr<AGun> gun) { CurGun = gun; }