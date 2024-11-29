// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AoE/ColorboundAreaOfEffectActor.h"
#include "Characters/ColorboundEnemyCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/SphereComponent.h"
#include "Subsystems/ObjectPoolWorldSubsystem.h"

// Sets default values
AColorboundAreaOfEffectActor::AColorboundAreaOfEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
		CollisionComponent->InitSphereRadius(100.0f);
		RootComponent = CollisionComponent;

		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
		CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
	}

}

// Called when the game starts or when spawned
void AColorboundAreaOfEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AColorboundAreaOfEffectActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AColorboundEnemyCharacter* Target = Cast<AColorboundEnemyCharacter>(OtherActor))
	{
		UAbilitySystemComponent* ASC = Target->GetAbilitySystemComponent();
		if (ASC && DamageSpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle AppliedGameplayEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*DamageSpecHandle.Data.Get());
			if (DamageSpecHandle.Data.Get()->Duration == FGameplayEffectConstants::INFINITE_DURATION)
			{
				ActiveGameplayEffectHandles.Add(Target, AppliedGameplayEffectHandle);
			}
		}
	}
}

void AColorboundAreaOfEffectActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AColorboundEnemyCharacter* Target = Cast<AColorboundEnemyCharacter>(OtherActor))
	{
		UAbilitySystemComponent* ASC = Target->GetAbilitySystemComponent();
		FActiveGameplayEffectHandle AppliedGameplayEffectHandle = ActiveGameplayEffectHandles.FindAndRemoveChecked(Target);
		if (ASC)
		{
			ASC->RemoveActiveGameplayEffect(AppliedGameplayEffectHandle);
		}
	}
}

void AColorboundAreaOfEffectActor::OnSpawnFromPool_Implementation()
{
}

void AColorboundAreaOfEffectActor::OnReturnToPool_Implementation()
{
	for (TPair<AColorboundEnemyCharacter*, FActiveGameplayEffectHandle>& ActiveGameplayEffectHandle : ActiveGameplayEffectHandles)
	{
		UAbilitySystemComponent* ASC = ActiveGameplayEffectHandle.Key->GetAbilitySystemComponent();
		FActiveGameplayEffectHandle AppliedGameplayEffectHandle = ActiveGameplayEffectHandle.Value;
		if (ASC)
		{
			ASC->RemoveActiveGameplayEffect(AppliedGameplayEffectHandle);
		}
	}
}

