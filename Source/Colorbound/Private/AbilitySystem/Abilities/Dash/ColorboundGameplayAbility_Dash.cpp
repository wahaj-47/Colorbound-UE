// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Dash/ColorboundGameplayAbility_Dash.h"
#include "Core/ColorboundCharacterBase.h"
#include "Components/CapsuleComponent.h"

UColorboundGameplayAbility_Dash::UColorboundGameplayAbility_Dash(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UColorboundGameplayAbility_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	CharacterDashStop();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UColorboundGameplayAbility_Dash::CharacterDashStart()
{
	if (AColorboundCharacterBase* ColorboundCharacter = GetColorboundCharacterFromActorInfo())
	{
		ColorboundCharacter->GetCapsuleComponent()->SetCollisionProfileName(FName("Ghost"));
	}
}

void UColorboundGameplayAbility_Dash::CharacterDashStop()
{
	if (AColorboundCharacterBase* ColorboundCharacter = GetColorboundCharacterFromActorInfo())
	{
		ColorboundCharacter->GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn"));
	}
}

FVector UColorboundGameplayAbility_Dash::GetFurthestLocationAlongPath() const
{
	AColorboundCharacterBase* Character = GetColorboundCharacterFromActorInfo();
	FVector Start = Character->GetActorLocation();
	FVector End = Start + (Character->GetActorForwardVector() * MaxDashDistance);

	FVector Direction = (End - Start).GetSafeNormal();
	float CapsuleRadius;
	float CapsuleHalfHeight;
	Character->GetCapsuleComponent()->GetScaledCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
	float CapsuleDiameter = CapsuleRadius * 2.f;
	int CapsuleCount = MaxDashDistance / CapsuleDiameter;

	// Trace parameters
	FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams::AllObjects;
	ObjectQueryParams.RemoveObjectTypesToQuery(ECollisionChannel::ECC_PhysicsBody);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	for (int i = CapsuleCount; i > 0; i--)
	{
		FVector TraceLocation = Start + (i * CapsuleDiameter * Direction);
		TArray<FHitResult> OutHits;
		bool bCapsuleHit = GetWorld()->SweepMultiByObjectType(
			OutHits,
			TraceLocation,
			TraceLocation,
			FQuat::Identity,
			ObjectQueryParams,
			FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
			QueryParams
		);

		FHitResult OutHit;
		bool bLineHit = GetWorld()->LineTraceSingleByChannel(
			OutHit,  // Result
			TraceLocation,   // Start location
			TraceLocation - FVector(0,0,500),     // End location
			ECollisionChannel::ECC_Visibility,  // Collision channel
			QueryParams  // Query parameters
		);

		if (bLineHit)
		{
			if (!bCapsuleHit)
			{
				return TraceLocation;
			}

			bool bIsBlockingPawn = false;
			for (const FHitResult& Hit : OutHits)
			{
				if (Hit.Component->GetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn) == ECollisionResponse::ECR_Block)
				{
					bIsBlockingPawn = true;
					break;
				}
			}

			if (!bIsBlockingPawn)
			{
				return TraceLocation;
			}
		}
		
	}

	return Start;
}
