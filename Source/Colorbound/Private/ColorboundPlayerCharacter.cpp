// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorboundPlayerCharacter.h"
#include "ColorboundPlayerState.h"
#include "ColorboundAbilitySystemComponent.h"
#include "ColorboundAttributeSet.h"
#include "ColorboundInputComponent.h"
#include "ColorboundGameplayTags.h"
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

	ColorboundInputComponent->BindNativeAction(InputConfig, ColorboundGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
}

void AColorboundPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AColorboundPlayerCharacter::InputAbilityInputTagPressed(FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityInputTagPressed(InputTag);
}

void AColorboundPlayerCharacter::InputAbilityInputTagReleased(FGameplayTag InputTag)
{
	AbilitySystemComponent->AbilityInputTagReleased(InputTag);
}

void AColorboundPlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	FVector2D InputVector = InputActionValue.Get<FVector2D>();
	
	ForwardVector = FVector2D(InputVector.Y, InputVector.X);
	RightVector = ForwardVector.GetRotated(90);
	
	AddMovementInput(FVector::RightVector, InputVector.X);
	AddMovementInput(FVector::ForwardVector, InputVector.Y);
}
