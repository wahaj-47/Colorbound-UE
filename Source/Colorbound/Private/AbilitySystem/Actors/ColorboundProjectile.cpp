// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Actors/ColorboundProjectile.h"
#include "Characters/ColorboundEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Subsystems/ObjectPoolWorldSubsystem.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AColorboundProjectile::AColorboundProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
		CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
		CollisionComponent->InitSphereRadius(15.0f);
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if (!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
		MeshComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void AColorboundProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AColorboundProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AColorboundEnemyCharacter* Target = Cast<AColorboundEnemyCharacter>(OtherActor);
	if (Target)
	{
		UAbilitySystemComponent* ASC = Target->GetAbilitySystemComponent();
		if (ASC && DamageSpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*DamageSpecHandle.Data.Get());
		}
	}

	UObjectPoolWorldSubsystem* ObjectPoolWorldSubsystem = GetWorld()->GetSubsystem<UObjectPoolWorldSubsystem>();
	ObjectPoolWorldSubsystem->ReturnToPool(this);
}

void AColorboundProjectile::OnSpawnFromPool_Implementation()
{
	// ProjectileMovementComponent sets it to NULL when velocity is less than threshold
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector::ForwardVector * ProjectileMovementComponent->InitialSpeed);
}

void AColorboundProjectile::OnReturnToPool_Implementation()
{
	ProjectileMovementComponent->StopMovementImmediately();
}

