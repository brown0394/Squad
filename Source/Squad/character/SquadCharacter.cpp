// Copyright Epic Games, Inc. All Rights Reserved.

#include "SquadCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "../weapon/Gun.h"
#include "SquadPlayerController.h"
#include "AICharacter.h"
#include "Components/PointLightComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASquadCharacter

ASquadCharacter::ASquadCharacter() : bOrdering(false), bClicked(false), bMemberSelected(false), _memberIdx(-1)
{
	PrimaryActorTick.bCanEverTick = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	TObjectPtr<USkeletalMeshComponent> skeletalMC = this->GetComponentByClass<USkeletalMeshComponent>();
	if (skeletalMC != NULL) {
		FollowCamera->SetupAttachment(skeletalMC, FName(TEXT("headSocket")));
	}
	//FollowCamera->SetupAttachment(RootComponent);
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	// Create ordering light component (hidden by default)
	OrderingLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("OrderingLight"));
	OrderingLight->SetupAttachment(RootComponent);
	OrderingLight->SetIntensity(1000.0f);
	OrderingLight->SetLightColor(FLinearColor(0.0f, 0.0f, 0.4f));
	OrderingLight->SetVisibility(false);
	OrderingLight->SetMobility(EComponentMobility::Movable);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASquadCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	SquadPlayerController = Cast<ASquadPlayerController>(GetController());
	SquadPlayerController->Owner = this;
	SquadPlayerController->InitSquad();
}

void ASquadCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bOrdering)
	{
		UpdateOrderingLight();
	}
}

void ASquadCharacter::UpdateOrderingLight()
{
	FHitResult Hit;
	TraceForward(Hit, 2000.0f, false);

	if (Hit.bBlockingHit == false)
	{
		OrderingLight->SetVisibility(false);
		return;
	}

	FVector LightLocation = Hit.Location + Hit.ImpactNormal * 10.0f;
	OrderingLight->SetWorldLocation(LightLocation);
	OrderingLight->SetVisibility(true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASquadCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASquadCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASquadCharacter::Look);

		//Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ASquadCharacter::Aim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ASquadCharacter::StopAiming);

		// Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ASquadCharacter::Interact);

		// Using Weapon
		EnhancedInputComponent->BindAction(UseWeaponAction, ETriggerEvent::Triggered, this, &ASquadCharacter::TriggerUseWeapon);

		// Reloading
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ASquadCharacter::Reload);
		
		// Ordering
		EnhancedInputComponent->BindAction(OrderAction, ETriggerEvent::Triggered, this, &ASquadCharacter::Order);

		EnhancedInputComponent->BindAction(NumPressedAction1, ETriggerEvent::Triggered, this, &ASquadCharacter::NumPressed1);
		EnhancedInputComponent->BindAction(NumPressedAction2, ETriggerEvent::Triggered, this, &ASquadCharacter::NumPressed2);
		EnhancedInputComponent->BindAction(NumPressedAction3, ETriggerEvent::Triggered, this, &ASquadCharacter::NumPressed3);
		EnhancedInputComponent->BindAction(NumPressedAction4, ETriggerEvent::Triggered, this, &ASquadCharacter::NumPressed4);
		EnhancedInputComponent->BindAction(NumPressedAction5, ETriggerEvent::Triggered, this, &ASquadCharacter::NumPressed5);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ASquadCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASquadCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASquadCharacter::TraceForward(FHitResult& Hit, const float dist, const bool debug) {
	FVector TraceStart = FollowCamera->GetComponentLocation();
	FVector TraceEnd = TraceStart + FollowCamera->GetForwardVector() * dist;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, QueryParams);

	if (debug)
	{
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 3.0f);
		UE_LOG(LogTemp, Log, TEXT("Tracing line: %s to %s"), *TraceStart.ToCompactString(), *TraceEnd.ToCompactString());
	}
}

void ASquadCharacter::Interact() {
	FHitResult Hit;
	TraceForward(Hit, 200.0f, true);
	IInteract* interactTarget = Cast<IInteract>(Hit.GetActor());
	if (interactTarget == nullptr) return;
	interactTarget->Interact(this);
	IsAttacking = false;
	IsReloading = false;
	OnAttackingStateChange.Broadcast();
	OnReloadStateChange.Broadcast();
}

void ASquadCharacter::TriggerUseWeapon() {
	if (bOrdering) return;
	UseWeapon();
}

void ASquadCharacter::Order() {
	if (bOrdering) {
		bOrdering = false;
		OrderingLight->SetVisibility(false);
		SquadPlayerController->MakeOrderUI(false);
		SquadPlayerController->CrosshairOnOff(false);
		return;
	}

	bOrdering = true;
	_memberIdx = -1;
	SquadPlayerController->MakeOrderUI(true);
	SquadPlayerController->CrosshairOnOff(true);
}

void ASquadCharacter::OrderNum(int num) {
	if (!bOrdering)
		return;

	if (_memberIdx < 0) {
		SelectMember(num);
		return;
	}
	SelectOrder(num);
}

void ASquadCharacter::NumPressed1() {
	OrderNum(0);
}
void ASquadCharacter::NumPressed2() {
	OrderNum(1);
}
void ASquadCharacter::NumPressed3() {
	OrderNum(2);
}
void ASquadCharacter::NumPressed4() {
	OrderNum(3);
}
void ASquadCharacter::NumPressed5() {
	OrderNum(4);
}

void ASquadCharacter::SelectMember(int memberIdx) {
	_memberIdx = memberIdx;
	SquadPlayerController->MakeOrderUI(false);
	SquadPlayerController->OrderListOnOff(true);
}

void ASquadCharacter::SelectOrder(int orderIdx) {
	switch (orderIdx) {
	case 0: { DesignateTarget(); break; }
	case 1: { FollowOrder(); break; }
	case 2: { MoveToPositionOrder(); break; }
	case 4: { FreeWillOrder(); break; }
	}
	_memberIdx = -1;
	bOrdering = false;
	OrderingLight->SetVisibility(false);
	SquadPlayerController->MakeOrderUI(false);
	SquadPlayerController->OrderListOnOff(false);
	SquadPlayerController->CrosshairOnOff(false);
}

void ASquadCharacter::DesignateTarget() {
	FHitResult Hit;
	TraceForward(Hit, 2000.0f, true);
	SquadPlayerController->SetMemberTarget(Hit.GetActor(), _memberIdx);
}

void ASquadCharacter::FollowOrder() {
	SquadPlayerController->SetMemberFollow(_memberIdx);
}

void ASquadCharacter::MoveToPositionOrder() {
	FHitResult Hit;
	TraceForward(Hit, 2000.0f, true);
	if (Hit.bBlockingHit)
		SquadPlayerController->SetMemberMoveToPosition(Hit.Location, _memberIdx);
}

void ASquadCharacter::FreeWillOrder() {
	SquadPlayerController->SetMemberFreeWill(_memberIdx);
}