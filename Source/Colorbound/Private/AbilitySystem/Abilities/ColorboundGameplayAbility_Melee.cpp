// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ColorboundGameplayAbility_Melee.h"
#include "ColorboundCharacterBase.h"
#include "ColorboundEnemyCharacter.h"
#include "ColorboundAbilitySystemComponent.h"
#include "PaperZDAnimationComponent.h"
#include "PaperZDAnimInstance.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

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

void UColorboundGameplayAbility_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (bHasBlueprintActivate)
	{
		K2_ActivateAbility();
	}

	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (AColorboundCharacterBase* ColorboundCharacter = GetColorboundCharacterFromActorInfo())
		{
			if (ColorboundCharacter->IsLocallyControlled())
			{
				ColorboundCharacter->GetAnimationComponent()->GetAnimInstance()->PlayAnimationOverride(AttackAnimations[AnimationIndex], FName("DefaultSlot"), 1.0f, 0.0f, FZDOnAnimationOverrideEndSignature::CreateUObject(this, &ThisClass::OnAnimationEnd));
				
				UAbilityTask_WaitGameplayEvent* WaitForEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag);
				if (WaitForEventTask)
				{
					WaitForEventTask->EventReceived.AddDynamic(this, &ThisClass::PerformHitScan);
					WaitForEventTask->ReadyForActivation();
				}

				AnimationIndex++;
				if (AnimationIndex > AttackAnimations.Num() - 1)
				{
					AnimationIndex = 0;
				}
			}
		}
	}
}

void UColorboundGameplayAbility_Melee::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UColorboundGameplayAbility_Melee::OnAnimationEnd(bool bWasCompleted)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, !bWasCompleted);
}

void UColorboundGameplayAbility_Melee::PerformHitScan(FGameplayEventData Payload)
{
	if (AColorboundCharacterBase* Owner = GetColorboundCharacterFromActorInfo())
	{
		if (Owner->IsLocallyControlled())
		{
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(Owner); // Optionally ignore the actor performing the trace
			FVector Location = Owner->GetActorLocation() + (Owner->GetForwardVector() * Range);

			bool bHit = GetWorld()->SweepSingleByChannel(
				HitResult,
				Location,
				Location,
				FQuat::Identity,
				ECC_Pawn,  // Change to the collision channel you're interested in
				FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
				QueryParams
			);

			if (bHit)  // Check if something was hit and if there is a valid actor
			{
				if (AColorboundEnemyCharacter* Target = Cast<AColorboundEnemyCharacter>(HitResult.GetActor()))
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

