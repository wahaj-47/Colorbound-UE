// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorboundPlayerState.h"
#include "ColorboundAbilitySystemComponent.h"
#include "ColorboundAttributeSet.h"

AColorboundPlayerState::AColorboundPlayerState()
{
	NetUpdateFrequency = 100.f;

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UColorboundAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSet = CreateDefaultSubobject<UColorboundAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AColorboundPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UColorboundAttributeSet* AColorboundPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
