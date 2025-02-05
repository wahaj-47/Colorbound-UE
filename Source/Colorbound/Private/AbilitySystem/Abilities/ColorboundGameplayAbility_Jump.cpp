// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ColorboundGameplayAbility_Jump.h"
#include "Core/ColorboundCharacterBase.h"

UColorboundGameplayAbility_Jump::UColorboundGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UColorboundGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const AColorboundCharacterBase* ColorboundCharacter = Cast<AColorboundCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!ColorboundCharacter || !ColorboundCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UColorboundGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Stop jumping in case the ability blueprint doesn't call it.
	CharacterJumpStop();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UColorboundGameplayAbility_Jump::CharacterJumpStart()
{
	if (AColorboundCharacterBase* ColorboundCharacter = GetColorboundCharacterFromActorInfo())
	{
		if (ColorboundCharacter->IsLocallyControlled() && !ColorboundCharacter->bPressedJump)
		{
			ColorboundCharacter->UnCrouch();
			ColorboundCharacter->Jump();
		}
	}
}

void UColorboundGameplayAbility_Jump::CharacterJumpStop()
{
	if (AColorboundCharacterBase* ColorboundCharacter = GetColorboundCharacterFromActorInfo())
	{
		if (ColorboundCharacter->IsLocallyControlled() && ColorboundCharacter->bPressedJump)
		{
			ColorboundCharacter->StopJumping();
		}
	}
}
