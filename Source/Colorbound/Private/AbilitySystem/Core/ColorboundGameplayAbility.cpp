// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Core/ColorboundGameplayAbility.h"
#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"
#include "AbilitySystem/Core/ColorboundAbilityTypes.h"
#include "Core/ColorboundCharacterBase.h"

UColorboundGameplayAbility::UColorboundGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UColorboundGameplayAbility::TryActivateAbilityOnGranted(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);

	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && !bIsPredicting && (ActivationPolicy == EColorboundAbilityActivationPolicy::OnGranted))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

void UColorboundGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	K2_OnAbilityAdded();

	TryActivateAbilityOnGranted(ActorInfo, Spec);
}

AColorboundCharacterBase* UColorboundGameplayAbility::GetColorboundCharacterFromActorInfo() const
{
    return (CurrentActorInfo ? Cast<AColorboundCharacterBase>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UColorboundAbilitySystemComponent* UColorboundGameplayAbility::GetColorboundAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UColorboundAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

const FColorboundGameplayAbilityActorInfo* UColorboundGameplayAbility::GetColorboundActorInfo(const FGameplayAbilityActorInfo* InActorInfo)
{
	return static_cast<const FColorboundGameplayAbilityActorInfo*>(InActorInfo);
}

float UColorboundGameplayAbility::GetScalableFloatValueAtLevel(const FScalableFloat& ScalableFloat, int32 Level) const
{
	return ScalableFloat.GetValueAtLevel(Level);
}

UPaperZDAnimSequence* UColorboundGameplayAbility::GetCurrentSequence() const
{
	return CurrentSequence;
}

void UColorboundGameplayAbility::SetCurrentSequence(UPaperZDAnimSequence* InCurrentSequence)
{
	if (!ensure(IsInstantiated()))																														\
	{																																					\
		UE_LOG(LogTemp, Error, TEXT("%s: SetCurrentSequence cannot be called on a non-instanced ability. Check the instancing policy."), *GetPathName());	\
		return;																																\
	}
	CurrentSequence = InCurrentSequence;
}
