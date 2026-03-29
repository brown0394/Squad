// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../weapon/Gun.h"
#include "HealthComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter() : IsAiming(false), IsAttacking(false), IsReloading(false), IsSprinting(false), RecoilToApply(FRotator::ZeroRotator)
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

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComp->OnDeath.AddUObject(this, &ABaseCharacter::Death);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	//spawn default gun and attach it to character
	GetWorld()->SpawnActor<AGun>(GunToSpawn, FVector::ZeroVector, FRotator::ZeroRotator)->Interact(this);
}

void ABaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	OnAimingStateChange.Clear();
	OnAttackingStateChange.Clear();
	OnReloadStateChange.Clear();
	OnSprintStateChange.Clear();
	HealthComp->DestroyComponent();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!RecoilToApply.IsNearlyZero(0.01f)) {
		float recoilSpeed = (CurGun != nullptr) ? CurGun->GetRecoilSpeed() : 10.f;
		FRotator step = FMath::RInterpTo(FRotator::ZeroRotator, RecoilToApply, DeltaTime, recoilSpeed);
		RecoilToApply -= step;

		TObjectPtr<AController> CharController = GetController();
		if (CharController != nullptr) {
			CharController->SetControlRotation(CharController->GetControlRotation() + step);
		}
	}
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCharacter::Aim() {
	if (!IsAiming) {
		if (IsSprinting) {
			StopSprinting();
		}
		IsAiming = true;
		GetCharacterMovement()->MaxWalkSpeed = AimSpeed;
		OnAimingStateChange.Broadcast();
	}
}

void ABaseCharacter::StopAiming() {
	if (IsAiming) {
		IsAiming = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		OnAimingStateChange.Broadcast();
	}
}

void ABaseCharacter::Sprint() {
	if (IsAiming) return;
	if (!IsSprinting) {
		IsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		OnSprintStateChange.Broadcast();
	}
}

void ABaseCharacter::StopSprinting() {
	if (IsSprinting) {
		IsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		OnSprintStateChange.Broadcast();
	}
}

bool ABaseCharacter::GetIsSprinting() {
	return IsSprinting;
}

bool ABaseCharacter::UseWeapon() {
	if (CurGun == nullptr || IsReloading || CurGun->GetBulletsLeft() == 0) return false;
	if (!IsAttacking && CurGun->DoAttack()) {
		IsAttacking = true;
		OnAttackingStateChange.Broadcast();
		MakeNoise(1.0f, this, GetActorLocation());

		RecoilToApply += FRotator(CurGun->GetRecoilPitch(), FMath::RandRange(-CurGun->GetRecoilYaw(), CurGun->GetRecoilYaw()), 0.f);
	}
	return true;
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
	if (CurGun == nullptr)
		return;

	CurGun->SetBulletsLeft(Magazines[CurGun->GetBulletType()].innerArray.Pop());
}

float ABaseCharacter::GetFireRate()
{
	if (CurGun == nullptr ) return 0.f;

	return CurGun->GetAttackRate();
}

TObjectPtr<AGun> ABaseCharacter::GetCurGun() { return CurGun; };
void ABaseCharacter::SetCurGun(TObjectPtr<AGun> gun) { CurGun = gun; }

void ABaseCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID) {
	TeamId = InTeamID;
}
FGenericTeamId ABaseCharacter::GetGenericTeamId() const { return TeamId; }

void ABaseCharacter::Death() {
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetGenericTeamId(FGenericTeamId::NoTeam);
	EndPlay(EEndPlayReason::Destroyed);
}

const TObjectPtr<class UHealthComponent> ABaseCharacter::GetHealthComponent() {
	return HealthComp;
}