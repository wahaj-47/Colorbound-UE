// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/ColorboundPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Core/ColorboundPlayerState.h"
#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"
#include "AbilitySystem/Core/ColorboundAttributeSet.h"
#include "AbilitySystem/ActorComponents/AbilityQueueComponent.h"
#include "Input/ColorboundInputComponent.h"
#include "Input/ColorboundInputComponent.h"
#include "ColorboundGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void AColorboundPlayerCharacter::InitAbilitySystemComponent()
{
	AColorboundPlayerState* PS = GetPlayerState<AColorboundPlayerState>();
	check(PS);

	AbilitySystemComponent = CastChecked<UColorboundAbilitySystemComponent>(PS->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	AttributeSet = PS->GetAttributeSet();
	AbilityQueueComponent->InitAbilityQueueComponent(AbilitySystemComponent);
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

AColorboundPlayerCharacter::AColorboundPlayerCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 1080.f);

	bUseControllerRotationYaw = false;

	AbilityQueueComponent = CreateDefaultSubobject<UAbilityQueueComponent>(TEXT("AbilityQueueComponent"));
}

void AColorboundPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UColorboundInputComponent* ColorboundInputComponent = Cast<UColorboundInputComponent>(PlayerInputComponent);
	check(ColorboundInputComponent);

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (DefaultMappingContext)
	{
		// Add the mapping context with priority 0
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

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
	
	AddMovementInput(FVector::RightVector, InputVector.X);
	AddMovementInput(FVector::ForwardVector, InputVector.Y);
}
