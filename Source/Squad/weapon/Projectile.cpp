// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "../character/HealthComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile() : damage(0)
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something blocking
	CollisionComp->SetRelativeScale3D({ 0.5f, 0.5f, 0.5f });

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 30000.f;
	ProjectileMovement->MaxSpeed = 30000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;

	// Die after 5 seconds by default
	InitialLifeSpan = 5.0f;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (ProjectileParticleSystem != nullptr) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileParticleSystem, GetActorLocation());
		}
		TObjectPtr<UHealthComponent> HpComp = OtherActor->GetComponentByClass<UHealthComponent>();
		if (HpComp != nullptr) HpComp->TakeDamage(damage, GetActorLocation());
		Destroy();
	}
}
