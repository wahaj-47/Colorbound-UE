// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorboundPlayerCharacter.h"
#include "ColorboundPlayerState.h"
#include "ColorboundAbilitySystemComponent.h"
#include "ColorboundAttributeSet.h"
#include "ColorboundInputComponent.h"

void AColorboundPlayerCharacter::InitAbilitySystemComponent()
{
	AColorboundPlayerState* PS = GetPlayerState<AColorboundPlayerState>();
	check(PS);

	AbilitySystemComponent = CastChecked<UColorboundAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
	AttributeSet = PS->GetAttributeSet();
}

void AColorboundPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilitySystemComponent();

	InitializeAbilitySet();
}

void AColorboundPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilitySystemComponent();
	InitializeAbilitySet();
}

void AColorboundPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UColorboundInputComponent* ColorboundInputComponent = Cast<UColorboundInputComponent>(PlayerInputComponent);
	check(ColorboundInputComponent);

	TArray<uint32> BindHandles;

	ColorboundInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::InputAbilityInputTagPressed,
		&ThisClass::InputAbilityInputTagReleased, /*out*/ BindHandles);
}

void AColorboundPlayerCharacter::InputAbilityInputTagPressed(FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityInputTagPressed(InputTag);
}

void AColorboundPlayerCharacter::InputAbilityInputTagReleased(FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityInputTagReleased(InputTag);
}
