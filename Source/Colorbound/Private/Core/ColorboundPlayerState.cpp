// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/ColorboundPlayerState.h"
#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"
#include "AbilitySystem/Core/ColorboundAttributeSet.h"
#include "Net/UnrealNetwork.h"

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

void AColorboundPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AColorboundPlayerState, PlayerLevel);
	DOREPLIFETIME(AColorboundPlayerState, PlayerXP);

}

UAbilitySystemComponent* AColorboundPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UColorboundAttributeSet* AColorboundPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AColorboundPlayerState::OnRep_PlayerLevel(int32 OldLevel)
{
	OnPlayerLevelChangedDelegate.Broadcast(PlayerLevel);
}

void AColorboundPlayerState::OnRep_PlayerXP(int32 OldXP)
{
	OnPlayerXPChangedDelegate.Broadcast(PlayerXP);
}

void AColorboundPlayerState::SetPlayerLevel(int32 InPlayerLevel)
{
	PlayerLevel = InPlayerLevel;
	OnPlayerLevelChangedDelegate.Broadcast(PlayerLevel);
}

void AColorboundPlayerState::SetPlayerXP(int32 InPlayerXP)
{
	PlayerXP = InPlayerXP;
	OnPlayerXPChangedDelegate.Broadcast(PlayerXP);
}
