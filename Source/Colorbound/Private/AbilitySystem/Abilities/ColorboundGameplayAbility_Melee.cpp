// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ColorboundGameplayAbility_Melee.h"
#include "ColorboundCharacterBase.h"
#include "ColorboundEnemyCharacter.h"
#include "ColorboundAbilitySystemComponent.h"
#include "PaperZDAnimationComponent.h"
#include "PaperZDAnimInstance.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

UColorboundGameplayAbility_Melee::UColorboundGameplayAbility_Melee(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UColorboundGameplayAbility_Melee::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UColorboundGameplayAbility_Melee::ApplyDamage(FGameplayEventData Payload)
{
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
				ECC_Pawn,  // Change to the collision channel you're interested in
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
							UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
							FGameplayEffectSpecHandle DamageSpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ASC->MakeEffectContext());
							DamageSpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, Damage);
							ASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), Target->GetAbilitySystemComponent());
						}
					}
				}
			}
		}
	}
}

