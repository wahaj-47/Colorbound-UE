// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ColorboundAbilityTypes.h"
#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"
#include "Core/ColorboundPlayerController.h"
#include "Core/ColorboundPlayerState.h"
#include "PaperZDAnimationComponent.h"

void FColorboundGameplayAbilityActorInfo::InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor, UAbilitySystemComponent* InAbilitySystemComponent)
{
	Super::InitFromActor(InOwnerActor, InAvatarActor, InAbilitySystemComponent);

	ColorboundAbilitySystemComponent = Cast<UColorboundAbilitySystemComponent>(InAbilitySystemComponent);
	
	if (AActor* const AvatarActorPtr = AvatarActor.Get())
	{
		// Grab Components that we care about
		ColorboundAnimationComponent = AvatarActorPtr->FindComponentByClass<UPaperZDAnimationComponent>();
	}

	if (PlayerController.IsValid())
	{
		ColorboundPlayerController = Cast<AColorboundPlayerController>(PlayerController.Get());
		ColorboundPlayerState = PlayerController->GetPlayerState<AColorboundPlayerState>();
	}
}

void FColorboundGameplayAbilityActorInfo::SetAvatarActor(AActor* InAvatarActor)
{
	Super::SetAvatarActor(InAvatarActor);
}

void FColorboundGameplayAbilityActorInfo::ClearActorInfo()
{
	Super::ClearActorInfo();

	ColorboundAbilitySystemComponent = nullptr;
	ColorboundPlayerController = nullptr;
	ColorboundPlayerState = nullptr;
}

AColorboundPlayerController* FColorboundGameplayAbilityActorInfo::GetColorboundPlayerController() const
{
	return ColorboundPlayerController.Get();
}

AColorboundPlayerState* FColorboundGameplayAbilityActorInfo::GetColorboundPlayerState() const
{
	return ColorboundPlayerState.Get();
}

UPaperZDAnimInstance* FColorboundGameplayAbilityActorInfo::GetAnimInstance() const
{
	UPaperZDAnimationComponent* AnimationComponent = ColorboundAnimationComponent.Get();
	if (AnimationComponent)
	{
		return AnimationComponent->GetAnimInstance();
	}

	return nullptr;
}
