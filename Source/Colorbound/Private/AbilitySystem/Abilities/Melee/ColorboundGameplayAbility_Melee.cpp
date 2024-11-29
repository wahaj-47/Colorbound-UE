// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Melee/ColorboundGameplayAbility_Melee.h"
#include "Core/ColorboundCharacterBase.h"
#include "Characters/ColorboundEnemyCharacter.h"

UColorboundGameplayAbility_Melee::UColorboundGameplayAbility_Melee(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

FGameplayAbilityTargetDataHandle UColorboundGameplayAbility_Melee::GetTargetData(FGameplayEventData Payload) const
{
	FGameplayAbilityTargetDataHandle Targets;

	if (AColorboundCharacterBase* Owner = GetColorboundCharacterFromActorInfo())
	{
		if (Owner->IsLocallyControlled())
		{
			TArray<FHitResult> HitResults;
			FCollisionQueryParams CapsuleQueryParams;
			CapsuleQueryParams.AddIgnoredActor(Owner); // Optionally ignore the actor performing the trace
			FVector Location = Owner->GetActorLocation() + (Owner->GetForwardVector() * Range);

			bool bHit = GetWorld()->SweepMultiByChannel(
				HitResults,
				Location,
				Location,
				FQuat::Identity,
				ECC_Pawn,
				FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
				CapsuleQueryParams
			);

			if (bHit)  // Check if something was hit and if there is a valid actor
			{
				for (const FHitResult& Hit : HitResults)
				{
					AActor* HitActor = Hit.GetActor();
					FHitResult HitResult;
					FCollisionQueryParams LineQueryParams;
					LineQueryParams.AddIgnoredActor(Owner);
					LineQueryParams.AddIgnoredActor(HitActor);

					bool bLineHit = GetWorld()->LineTraceSingleByChannel(
						HitResult,
						Location,
						Hit.ImpactPoint,
						ECC_Visibility,
						LineQueryParams
					);

					if (!bLineHit) {
						if (AColorboundEnemyCharacter* Target = Cast<AColorboundEnemyCharacter>(HitActor))
						{
							FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
							NewTargetData->HitResult = Hit;
							Targets.Add(NewTargetData);
						}
					}
				}
			}
		}
	}

	return Targets;
}

