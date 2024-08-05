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
#include "Gun.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASquadCharacter

ASquadCharacter::ASquadCharacter() : IsAiming(false)
{
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

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	TObjectPtr<USkeletalMeshComponent> skeletalMC = this->GetComponentByClass<USkeletalMeshComponent>();
	if (skeletalMC != NULL) {
		FollowCamera->SetupAttachment(skeletalMC, FName(TEXT("headSocket")));
	}
	//FollowCamera->SetupAttachment(RootComponent);
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	auto montage = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Mannequins/Animations/FPS/Fire_Rifle_Hip_Montage.Fire_Rifle_Hip_Montage'"));
	if (montage.Object != nullptr) {
		ShootAnimIdle = montage.Object;
	}
	montage = ConstructorHelpers::FObjectFinder<UAnimMontage>(TEXT("AnimMontage'/Game/Characters/Mannequins/Animations/FPS/Fire_Rifle_Ironsights_Montage.Fire_Rifle_Ironsights_Montage'"));
	if (montage.Object != nullptr) {
		ShootAnimIronsight = montage.Object;
	}
}

void ASquadCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	TObjectPtr<UWorld> const World = GetWorld();
	CurGun = World->SpawnActor<AGun>(GunToSpawn, FVector::ZeroVector, FRotator::ZeroRotator);
	CurGun->AttachWeapon(this);
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
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
		EnhancedInputComponent->BindAction(UseWeaponAction, ETriggerEvent::Triggered, this, &ASquadCharacter::UseWeapon);
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

void ASquadCharacter::Aim() {
	IsAiming = true;
	
	GetCharacterMovement()->MaxWalkSpeed = 250.f;
}

void ASquadCharacter::StopAiming() {
	IsAiming = false;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}

void ASquadCharacter::UseWeapon() {
	if (CurGun == nullptr) return;
	if (CurGun->DoAttack()) {
		if (IsAiming) PlayAnimMontage(ShootAnimIronsight);
		else PlayAnimMontage(ShootAnimIdle);
	}
}

bool ASquadCharacter::GetIsAiming() {
	return IsAiming;
}

void ASquadCharacter::Interact() {
	FHitResult Hit;
	
	FVector TraceStart = FollowCamera->GetComponentLocation();
	FVector TraceEnd = TraceStart + FollowCamera->GetForwardVector() * 200.0f;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, QueryParams);

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 3.0f);
	UE_LOG(LogTemp, Log, TEXT("Tracing line: %s to %s"), *TraceStart.ToCompactString(), *TraceEnd.ToCompactString());

	TObjectPtr<AGun> GunToPick = Cast<AGun>(Hit.GetActor());
	if (GunToPick != nullptr) {
		if (CurGun != nullptr) {
			TObjectPtr<UWorld> const World = GetWorld();
			TObjectPtr<AGun> spawned =
			World->SpawnActor<AGun>(CurGun.GetClass(), GetActorLocation() + (GetActorForwardVector() * 20), FRotator::ZeroRotator);
			spawned->SetBulletsLeft(CurGun->GetBulletsLeft());
			CurGun->Destroy();
		}
		CurGun = GunToPick;
		CurGun->AttachWeapon(this);
	}
	//FActorSpawnParameters ActorSpawnParams;
	//ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
}

void ASquadCharacter::Reload() {
	if (CurGun == nullptr) return;
	if (Magazines[CurGun->GetBulletType()].innerArray.Num() > 0) {

	}
}